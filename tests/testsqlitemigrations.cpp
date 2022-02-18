#include "testmigrations.h"
#include "../Firfuorida/migration.h"
#include "../Firfuorida/migrator.h"
#include <QObject>
#include <QTest>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QSqlDriver>

#include "migrations/m20220119t181049_tiny.h"
#include "migrations/m20220119t181249_small.h"
#include "migrations/m20220119t181401_medium.h"
#include "migrations/m20220119t181501_big.h"
#include "migrations/m20220120t145652_tests1.h"
#include "migrations/m20220121t083111_defaults.h"
#include "migrations/m20220129t115726_foreignkey1.h"
#include "migrations/m20220129t115731_foreignkey2.h"
#include "migrations/m20220218t084654_drop_column.h"

#define DB_CONN "sqlitemigtests"

class TestSqliteMigrations : public TestMigrations
{
    Q_OBJECT
public:
    TestSqliteMigrations(QObject *parent = nullptr) : TestMigrations(parent) {}
    ~TestSqliteMigrations() override = default;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testTinyCols();
    void testDefaultValues();
    void testMigration();
    void testForeignKeys();
    void testDropColumn();

private:
    Firfuorida::Migrator *m_testmigrator = nullptr;
    QTemporaryFile m_sqliteDbFile;
    QSqlDatabase m_db;

    bool startDb();
    bool tableExists(const QString &tableName) const;
    bool checkColumn(const QString &table, const QString &column, const QString &type, ColOpts options, const QVariant &defVal = QVariant()) const;
};

bool TestSqliteMigrations::startDb()
{
    if (!m_sqliteDbFile.open()) {
        qCritical() << "Can not open SQLite database file at" << m_sqliteDbFile.fileName();
        qDebug() << m_sqliteDbFile.errorString();
        return false;
    }

    const QString initConName = QStringLiteral(DB_CONN) + QStringLiteral("-initcon");
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), initConName);
        db.setDatabaseName(m_sqliteDbFile.fileName());
        if (!db.open()) {
            qCritical() << "Failed to open SQLite database connection to" << m_sqliteDbFile.fileName();
            qDebug() << db.lastError().text();
            return false;
        }

        qDebug() << "Successfully created SQLite database at" << m_sqliteDbFile.fileName();
    }
    QSqlDatabase::removeDatabase(initConName);

    return true;
}

void TestSqliteMigrations::initTestCase()
{
    if (!startDb()) {
        QFAIL("Failed to create SQLite database.");
    }

    m_testmigrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20220119t181049_Tiny(m_testmigrator);
    new M20220119T181249_Small(m_testmigrator);
    new M20220119T181401_Medium(m_testmigrator);
    new M20220119T181501_Big(m_testmigrator);

    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral(DB_CONN));
        db.setDatabaseName(m_sqliteDbFile.fileName());
        if (!db.open()) {
            qDebug() << db.lastError().text();
            QFAIL("Failed to establish database connection.");
        }
        QSqlQuery q(db);
        if (!q.exec(QStringLiteral("PRAGMA foreign_keys = ON;"))) {
            qDebug() << db.lastError().text();
            QFAIL("Failed to enable foreign key pragma on SQLite database.");
        }
    }
}

void TestSqliteMigrations::cleanupTestCase()
{
    QSqlDatabase::removeDatabase(QStringLiteral(DB_CONN));
}

bool TestSqliteMigrations::tableExists(const QString &tableName) const
{
    QSqlQuery q(QSqlDatabase::database(QStringLiteral(DB_CONN)));
    if (q.prepare(QStringLiteral("SELECT COUNT(*) FROM main.sqlite_schema WHERE type = 'table' AND tbl_name = :tableName"))) {
        q.bindValue(QStringLiteral(":tableName"), tableName);
        if (q.exec()) {
            if (q.next()){
                if (q.value(0).toInt() == 1) {
                    return true;
                } else {
                    qCritical() << "Query for existence of table" << tableName << "returned" << q.value(0).toInt() << "instead of 1.";
                }
            } else {
                qCritical() << "No records found when checking existence of table" << tableName;
            }
        } else {
            qCritical() << "Failed to execute query to check existence of table:" << q.lastError().text();
        }
    } else {
        qCritical() << "Failed to prepare query to check existence of table:" << q.lastError().text();
    }

    return false;
}

bool TestSqliteMigrations::checkColumn(const QString &table, const QString &column, const QString &type, ColOpts options, const QVariant &defVal) const
{
    QSqlQuery q(QSqlDatabase::database(QStringLiteral(DB_CONN)));
    if (q.prepare(QStringLiteral("SELECT * FROM pragma_table_info('%1') WHERE name = :column").arg(table))) {
        q.bindValue(QStringLiteral(":column"), column);
        if (q.exec()) {
            if (q.next()) {

                if (!q.value(QStringLiteral("type")).toString().toLower().contains(type.toLower())) {
                    return false;
                }

                const bool _nullable = q.value(QStringLiteral("notnull")).toInt() == 1;
                if (options.testFlag(Nullable) && _nullable) {
                    qDebug() << "Column" << column << "on table" << table << "is not nullable as expected.";
                    return false;
                }
                if (!options.testFlag(Nullable) && !_nullable) {
                    qDebug() << "Column" << column << "on table" << table << "is not not nullable as expected.";
                    return false;
                }

                const bool _primaryKey = q.value(QStringLiteral("pk")).toInt() == 1;
                if (options.testFlag(PrimaryKey) && !_primaryKey) {
                    qDebug() << "Column" << column << "on table" << table << "is not a primary key as expected.";
                    return false;
                }
                if (!options.testFlag(PrimaryKey) && _primaryKey) {
                    qDebug() << "Column" << column << "on table" << table << "is unexpectedly a primary key.";
                    return false;
                }

                if (defVal.isValid()) {
                    auto _colDef = q.value(QStringLiteral("dflt_value")).toString();
                    const auto _defVal = defVal.toString();
                    if (_colDef.startsWith(QLatin1Char('\'')) && _colDef.endsWith(QLatin1Char('\''))) {
                        _colDef.chop(1);
                        _colDef.remove(0,1);
                    }
                    if (_colDef != _defVal) {
                        qCritical("Unexpected default value: %s != %s", qUtf8Printable(_colDef), qUtf8Printable(_defVal));
                        return false;
                    }
                }

                return true;

            } else {
                qCritical() << "Can not find column" << column << "on table" << table;
            }
        } else {
            qCritical() << "Failed to execute database query to check column" << column << "on" << table << ":" << q.lastError().text();
        }
    } else {
        qCritical() << "Failed to prepare database query to check column" << column << "on" << table << ":" << q.lastError().text();
    }

    return false;
}

void TestSqliteMigrations::testTinyCols()
{
    auto migrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20220119t181049_Tiny(migrator);
    QVERIFY(migrator->migrate());
    QVERIFY(tableExists(QStringLiteral("tiny")));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("id"), QStringLiteral("integer"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyIntCol"), QStringLiteral("integer"), TestMigrations::Unique));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyBlobCol"), QStringLiteral("blob"), TestMigrations::Nullable));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("text"), TestMigrations::NoOptions, QStringLiteral("dummer schiss")));
    QVERIFY(migrator->rollback());
    QVERIFY(!tableExists(QStringLiteral("tiny")));
    QVERIFY(migrator->rollback());
    QVERIFY(!tableExists(QStringLiteral("tiny")));
}

void TestSqliteMigrations::testDefaultValues()
{
    auto migrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20220121T083111_Defaults(migrator);
    QVERIFY(migrator->migrate());
}

void TestSqliteMigrations::testMigration()
{
    QVERIFY(m_testmigrator->migrate());
    QVERIFY(tableExists(QStringLiteral("tiny")));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("id"), QStringLiteral("integer"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyIntCol"), QStringLiteral("integer"), TestMigrations::Unique));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyBlobCol"), QStringLiteral("blob"), TestMigrations::Nullable));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("text"), TestMigrations::NoOptions, QStringLiteral("dummer schiss")));
    QVERIFY(tableExists(QStringLiteral("small")));
    QVERIFY(checkColumn(QStringLiteral("small"), QStringLiteral("id"), QStringLiteral("integer"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement|TestMigrations::Unsigned));
    QVERIFY(tableExists(QStringLiteral("medium")));
    QVERIFY(checkColumn(QStringLiteral("medium"), QStringLiteral("id"), QStringLiteral("integer"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement|TestMigrations::Unsigned));
    QVERIFY(tableExists(QStringLiteral("big")));
    QVERIFY(checkColumn(QStringLiteral("big"), QStringLiteral("id"), QStringLiteral("integer"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement|TestMigrations::Unsigned));
}

void TestSqliteMigrations::testForeignKeys()
{
    auto migrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20220129T115726_Foreignkey1(migrator);
    new M20220129T115731_Foreignkey2(migrator);
    QVERIFY(migrator->migrate());
}

void TestSqliteMigrations::testDropColumn()
{
    auto migrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20220119t181049_Tiny(migrator);
    new M20220218T084654_Drop_column(migrator);
    QVERIFY(migrator->migrate());
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("colToDrop"), QStringLiteral("integer"), TestMigrations::NoOptions));
    QVERIFY(migrator->rollback());
}

QTEST_MAIN(TestSqliteMigrations)

#include "testsqlitemigrations.moc"
