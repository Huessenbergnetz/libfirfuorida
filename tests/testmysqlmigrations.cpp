#include "testmigrations.h"
#include "../Firfuorida/migration.h"
#include "../Firfuorida/migrator.h"
#include <QObject>
#include <QTest>
#include <QDebug>
#include <QProcess>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QRegularExpression>

#include "migrations/m20220119t181049_tiny.h"
#include "migrations/m20220119t181249_small.h"
#include "migrations/m20220119t181401_medium.h"
#include "migrations/m20220119t181501_big.h"

#define DB_NAME "mysqlmigtestdb"
#define DB_USER "mysqlmigtester"
#define DB_PASS "mysqllalalala1"
#define DB_CONN "mysqlmigtests"

class TestMySqlMigrations : public TestMigrations
{
    Q_OBJECT
public:
    TestMySqlMigrations(QObject *parent = nullptr) : TestMigrations(parent) {}
    ~TestMySqlMigrations() override = default;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testTinyCols();
    void testMigration();

private:
    Firfuorida::Migrator *m_testmigrator = nullptr;
    QTemporaryDir m_mysqlWorkingDir;
    QTemporaryDir m_mysqlDataDir;
    QTemporaryDir m_mysqlSocketDir;
    QTemporaryDir m_mysqlLogDir;
    QTemporaryFile m_mysqlConfigFile;
    QProcess m_mysqlProcess;
    const int m_mysqlPort{46000};

    bool startDb();
    bool stopDb();
    bool createDatabase(const QString &name, const QString &user, const QString &password);

    QString mysqlSocketPath() const;

    bool tableExists(const QString &tableName) const;

    bool checkTableEngine(const QString &tableName, const QString &engine) const;
    bool checkTableCollation(const QString &tableName, const QString &collation) const;
    bool checkTableComment(const QString &tableName, const QString &comment) const;

    bool checkColumn(const QString &table, const QString &column, const QString &type, ColOpts options = NoOptions, const QVariant &defVal = QVariant()) const;
};

bool TestMySqlMigrations::startDb()
{
    const QString mysqlExecutable = QStandardPaths::findExecutable(QStringLiteral("mysql"));
    if (mysqlExecutable.isEmpty()) {
        qCritical() << "Can not find mysql executable";
        return false;
    }

    QScopedPointer<QProcess> mysql(new QProcess(this));
    mysql->start(mysqlExecutable, QStringList({QStringLiteral("--version")}));
    if (mysql->waitForFinished()) {
        const QString versionLine = QString::fromLocal8Bit(mysql->readAllStandardOutput()).trimmed();
        if (!versionLine.isEmpty()) {

            QRegularExpression re;
            if (versionLine.contains(QLatin1String("mariadb"), Qt::CaseInsensitive)) {
                setDbType(Firfuorida::Migrator::MariaDB);
                re.setPattern(QStringLiteral(".*\\s+(\\d+\\.\\d+\\.\\d+)-[Mm]aria[Dd][Bb]"));
            } else {
                setDbType(Firfuorida::Migrator::MySQL);
                re.setPattern(QStringLiteral(".*Ver\\s+(\\d+\\.\\d+\\.\\d+)"));
            }

            QRegularExpressionMatch match = re.match(versionLine);
            if (match.hasMatch()) {
                auto _dbVersion = QVersionNumber::fromString(match.captured(1));
                if (_dbVersion.isNull()) {
                    qCritical() << "Can not determine database version.";
                    return false;
                } else {
                    setDbVersion(_dbVersion);
                }
                qDebug() << "Start initializing" << (dbType() == Firfuorida::Migrator::MariaDB ? "MariaDB" : "MySQL") << "server version" << dbVersion().toString();
            } else {
                qCritical() << "Can not determine MariaDB database version.";
                return false;
            }
        } else {
            qCritical() << "Can not check MySQL/MariaDB version";
            return false;
        }
    } else {
        qCritical() << "Can not check MySQL/MariaDB version";
        qDebug() << mysql->errorString();
        return false;
    }

    if (!m_mysqlWorkingDir.isValid()) {
        qCritical() << "Can not create mysql working directory.";
        return false;
    }

    if (!m_mysqlDataDir.isValid()) {
        qCritical() << "Can not create mysql data directory.";
        return false;
    }

    if (!m_mysqlSocketDir.isValid()) {
        qCritical() << "Can not create mysql socket directory.";
        return false;
    }

    if (!m_mysqlLogDir.isValid()) {
        qCritical() << "Can not create mysql log directory.";
        return false;
    }

    if (!m_mysqlConfigFile.open()) {
        qCritical() << "Can not open mysql config file.";
        return false;
    }

    {
        QTextStream mysqlConfOut(&m_mysqlConfigFile);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        mysqlConfOut.setCodec("UTF-8");
#endif
        mysqlConfOut << "[mysqld]" << '\n';
        mysqlConfOut << "bind-address=127.0.0.1" << '\n';
        mysqlConfOut << "log-error=" << m_mysqlLogDir.filePath(QStringLiteral("mysql.log")) << '\n';
        if (dbType() != Firfuorida::Migrator::MySQL || (dbType() == Firfuorida::Migrator::MySQL && dbVersion() < QVersionNumber(8,0,0))) {
            mysqlConfOut << "innodb_file_format=Barracuda" << '\n';
        }
        if (dbType() == Firfuorida::Migrator::MySQL && dbVersion() >= QVersionNumber(8,0,0)) {
            mysqlConfOut << "basedir=" << m_mysqlDataDir.path() << '\n';
            mysqlConfOut << "default_authentication_plugin=mysql_native_password" << '\n';
        }
        mysqlConfOut << "innodb_file_per_table=ON" << '\n';
        mysqlConfOut << "datadir=" << m_mysqlDataDir.path() << "/data" << '\n';
        mysqlConfOut << "port=" << m_mysqlPort << '\n';
        mysqlConfOut << "socket=" << m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock")) << '\n';
        mysqlConfOut << "server-id=1" << '\n';
        mysqlConfOut.flush();
    }

    const QString mysqlDefaultsArg = QStringLiteral("--defaults-file=") + m_mysqlConfigFile.fileName();
    QStringList mysqlInstallArgs{mysqlDefaultsArg};

    const QString mysqlDexecName = dbType() == Firfuorida::Migrator::MySQL ? QStringLiteral("mysqld") : QStringLiteral("mariadbd");
    QString mysqlServerCommand = QStandardPaths::findExecutable(mysqlDexecName, QStringList({QStringLiteral("/usr/sbin")}));
    if (mysqlServerCommand.isEmpty()) {
        mysqlServerCommand = QStandardPaths::findExecutable(mysqlDexecName);
    }

    if (mysqlServerCommand.isEmpty()) {
        qCritical() << "Can not find MySQL/MariaDB server executable.";
        return false;
    }

    QString dbInitCommand = QStandardPaths::findExecutable(QStringLiteral("mysql_install_db"));

    if (dbType() == Firfuorida::Migrator::MySQL && dbVersion() > QVersionNumber(5,7,6)) {
        mysqlInstallArgs.append(QStringLiteral("--initialize-insecure"));
        dbInitCommand = mysqlServerCommand;
    } else {
        mysqlInstallArgs.append(QStringLiteral("--force"));
        mysqlInstallArgs.append(QStringLiteral("--auth-root-authentication-method=normal"));
        mysqlInstallArgs.append(QStringLiteral("--skip-test-db"));
    }

    if (dbInitCommand.isEmpty()) {
        qCritical() << "Can not find MySQL/MariaDB server init executable.";
        return false;
    }

    qDebug() << "Executing:" << dbInitCommand << mysqlInstallArgs.join(QChar(QChar::Space));

    QScopedPointer<QProcess> mysqlInit(new QProcess(this));
    mysqlInit->start(dbInitCommand, mysqlInstallArgs);
    if (!mysqlInit->waitForFinished()) {
        qCritical() << "Can not install initial mysql database.";
        qDebug() << mysqlInit->errorString();
        qDebug() << mysqlInit->readAll();
        m_mysqlLogDir.setAutoRemove(false);
        return false;
    }

    qDebug() << "Finished initializing MySQL/MariaDB server.";
    if (mysqlInit->exitCode() != 0) {
        qCritical() << "Failed to initialize MySQL/MariaDB server.";
        qDebug() << QString::fromLocal8Bit(mysqlInit->readAll());
        m_mysqlLogDir.setAutoRemove(false);
        return false;
    }

    QFileInfo socketFi(m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock")));

    m_mysqlProcess.setArguments({mysqlDefaultsArg});
    m_mysqlProcess.setProgram(mysqlServerCommand);
    m_mysqlProcess.setWorkingDirectory(m_mysqlWorkingDir.path());
    qDebug() << "Executing:" << m_mysqlProcess.program() << m_mysqlProcess.arguments().join(QChar(QChar::Space));
    m_mysqlProcess.start();

    qDebug() << "Wating 5 seconds for the server to start up and socket to appear at" << socketFi.filePath();
    auto cur = QDateTime::currentDateTime();
    while (!socketFi.exists() && (cur.msecsTo(QDateTime::currentDateTime()) < 5000)) {

    }

    if (!socketFi.exists()) {
        qCritical() << "Failed to start mysql server within 5 seconds.";
        qDebug() << "MySQL startup params:" << m_mysqlProcess.arguments().join(QChar(QChar::Space));
        m_mysqlLogDir.setAutoRemove(false);
        return false;
    }

    qDebug() << "Successfully initialized" << (dbType() == Firfuorida::Migrator::MariaDB ? "MariaDB" : "MySQL") << "server version" << dbVersion().toString();
    qDebug() << "Config File:" << m_mysqlConfigFile.fileName();
    qDebug() << "Base Directory:" << m_mysqlDataDir.path();
    qDebug() << "Data Directory:" << m_mysqlDataDir.path() + "/data";
    qDebug() << "Log file:" << m_mysqlLogDir.filePath(QStringLiteral("mysql.log"));
    qDebug() << "Socket file:" << m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock"));

    return true;
}

bool TestMySqlMigrations::stopDb()
{
    m_mysqlProcess.terminate();
    return m_mysqlProcess.exitCode() == 0;
}

bool TestMySqlMigrations::createDatabase(const QString &name, const QString &user, const QString &password)
{
    const QString initConName = name + QStringLiteral("-initCon");
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), initConName);
        db.setDatabaseName(QStringLiteral("mysql"));
        db.setUserName(QStringLiteral("root"));
        db.setConnectOptions(QStringLiteral("UNIX_SOCKET=%1").arg(m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock"))));
        if (!db.open()) {
            qCritical() << "Failed to establish database connection:" << db.lastError().text();
            m_mysqlLogDir.setAutoRemove(false);
            return false;
        }

        QSqlQuery q(db);

        if (!q.exec(QStringLiteral("CREATE DATABASE %1").arg(name))) {
            qCritical() << "Failed to create database:" << q.lastError().text();
            m_mysqlLogDir.setAutoRemove(false);
            return false;
        }

        if (!q.exec(QStringLiteral("CREATE USER '%1'@'localhost' IDENTIFIED BY '%2'").arg(user, password))) {
            qCritical() << "Failed to create database user:" << q.lastError().text();
            m_mysqlLogDir.setAutoRemove(false);
            return false;
        }

        if (!q.exec(QStringLiteral("GRANT ALL ON %1.* TO '%2'@'localhost'").arg(name, user))) {
            qCritical() << "Failed to grant access to db user:" << q.lastError().text();
            m_mysqlLogDir.setAutoRemove(false);
            return false;
        }

        db.close();
        setDbName(name);
    }

    QSqlDatabase::removeDatabase(initConName);

    {
        setDbRootConn(name + QStringLiteral("-rootCon"));
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), dbRootConn());
        db.setDatabaseName(QStringLiteral("information_schema"));
        db.setUserName(QStringLiteral("root"));
        db.setConnectOptions(QStringLiteral("UNIX_SOCKET=%1").arg(mysqlSocketPath()));
        if (!db.open()) {
            qCritical() << "Failed to establish database connection:" << db.lastError().text();
            m_mysqlLogDir.setAutoRemove(false);
            return false;
        }
    }

    return true;
}

QString TestMySqlMigrations::mysqlSocketPath() const
{
    return m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock"));
}

bool TestMySqlMigrations::tableExists(const QString &tableName) const
{
    QSqlQuery q(QSqlDatabase::database(dbRootConn()));
    if (q.prepare(QStringLiteral("SELECT COUNT(*) FROM TABLES WHERE TABLE_SCHEMA = :db AND TABLE_NAME = :tableName"))) {
        q.bindValue(QStringLiteral(":db"), dbName());
        q.bindValue(QStringLiteral(":tableName"), tableName);
        if (q.exec() && q.next() && q.value(0).toInt() == 1) {
            return true;
        }
    }

    return false;
}

bool TestMySqlMigrations::checkTableEngine(const QString &tableName, const QString &engine) const
{
    QSqlQuery q(QSqlDatabase::database(dbRootConn()));
    if (q.prepare(QStringLiteral("SELECT ENGINE FROM TABLES WHERE TABLE_SCHEMA = :db AND TABLE_NAME = :tableName"))) {
        q.bindValue(QStringLiteral(":db"), dbName());
        q.bindValue(QStringLiteral(":tableName"), tableName);
        if (q.exec() && q.next()) {
            return q.value(0).toString() == engine;
        }
    }

    return false;
}

bool TestMySqlMigrations::checkTableCollation(const QString &tableName, const QString &collation) const
{
    QSqlQuery q(QSqlDatabase::database(dbRootConn()));
    if (q.prepare(QStringLiteral("SELECT TABLE_COLLATION FROM TABLES WHERE TABLE_SCHEMA = :db AND TABLE_NAME = :tableName"))) {
        q.bindValue(QStringLiteral(":db"), dbName());
        q.bindValue(QStringLiteral(":tableName"), tableName);
        if (q.exec() && q.next()) {
            return q.value(0).toString() == collation;
        }
    }

    return false;
}

bool TestMySqlMigrations::checkTableComment(const QString &tableName, const QString &comment) const
{
    QSqlQuery q(QSqlDatabase::database(dbRootConn()));
    if (q.prepare(QStringLiteral("SELECT TABLE_COMMENT FROM TABLES WHERE TABLE_SCHEMA = :db AND TABLE_NAME = :tableName"))) {
        q.bindValue(QStringLiteral(":db"), dbName());
        q.bindValue(QStringLiteral(":tableName"), tableName);
        if (q.exec() && q.next()) {
            return q.value(0).toString() == comment;
        }
    }

    return false;
}

bool TestMySqlMigrations::checkColumn(const QString &table, const QString &column, const QString &type, ColOpts options, const QVariant &defVal) const
{
    QSqlQuery q(QSqlDatabase::database(dbRootConn()));
    if (q.prepare(QStringLiteral("SELECT DATA_TYPE, IS_NULLABLE, COLUMN_KEY, EXTRA, COLUMN_TYPE, COLUMN_DEFAULT FROM COLUMNS WHERE TABLE_SCHEMA = :db AND TABLE_NAME = :table AND COLUMN_NAME = :column"))) {
        q.bindValue(QStringLiteral(":db"), dbName());
        q.bindValue(QStringLiteral(":table"), table);
        q.bindValue(QStringLiteral(":column"), column);
        if (q.exec() && q.next()) {

            if (q.value(0).toString().toLower() != type.toLower()) {
                return false;
            }

            const QString _nullable = options.testFlag(Nullable) ? QStringLiteral("yes") : QStringLiteral("no");
            if (q.value(1).toString().toLower() != _nullable) {
                return false;
            }

            const auto _colKey = q.value(2).toString();
            if (options.testFlag(PrimaryKey) && !_colKey.contains(QLatin1String("PRI"), Qt::CaseInsensitive)) {
                return false;
            } else if (!options.testFlag(PrimaryKey) && _colKey.contains(QLatin1String("PRI"), Qt::CaseInsensitive)) {
                return false;
            }
            if (options.testFlag(Unique) && !_colKey.contains(QLatin1String("UNI"), Qt::CaseInsensitive)) {
                return false;
            } else if (!options.testFlag(Unique) && _colKey.contains(QLatin1String("UNI"), Qt::CaseInsensitive)) {
                return false;
            }

            const auto _extra = q.value(3).toString();
            if (options.testFlag(AutoIncrement) && !_extra.contains(QLatin1String("auto_increment"), Qt::CaseInsensitive)) {
                return false;
            } else if (!options.testFlag(AutoIncrement) && _extra.contains(QLatin1String("auto_increment"), Qt::CaseInsensitive)) {
                return false;
            }

            const auto _colType = q.value(4).toString();
            if (options.testFlag(Unsigned) && !_colType.contains(QLatin1String("unsigned"), Qt::CaseInsensitive)) {
                return false;
            } else if (!options.testFlag(Unsigned) && _colType.contains(QLatin1String("unsigned"), Qt::CaseInsensitive)) {
                return false;
            }

            if (defVal.isValid()) {
                auto _colDef = q.value(5).toString();
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
        }
    }

    return false;
}

void TestMySqlMigrations::initTestCase()
{
    if (!startDb()) {
        QFAIL("Failed to start MySQL.");
    }
    if (!createDatabase(QStringLiteral(DB_NAME), QStringLiteral(DB_USER), QStringLiteral(DB_PASS))) {
        QFAIL("Failed to create database.");
    }

    m_testmigrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20220119t181049_Tiny(m_testmigrator);
    new M20220119T181249_Small(m_testmigrator);
    new M20220119T181401_Medium(m_testmigrator);
    new M20220119T181501_Big(m_testmigrator);

    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), QStringLiteral(DB_CONN));
        db.setDatabaseName(QStringLiteral(DB_NAME));
        db.setUserName(QStringLiteral(DB_USER));
        db.setPassword(QStringLiteral(DB_PASS));
        db.setHostName(QStringLiteral("127.0.0.1"));
        db.setPort(m_mysqlPort);
        if (!db.open()) {
            qDebug() << db.lastError().text();
            QFAIL("Failed to establish database connection.");
        }
    }
}

void TestMySqlMigrations::cleanupTestCase()
{
    QSqlDatabase::removeDatabase(QStringLiteral(DB_CONN));
}

void TestMySqlMigrations::testTinyCols()
{
    auto migrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20220119t181049_Tiny(migrator);
    QVERIFY(migrator->migrate());
    QVERIFY(tableExists(QStringLiteral("tiny")));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("id"), QStringLiteral("tinyint"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement|TestMigrations::Unsigned));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyIntCol"), QStringLiteral("tinyint"), TestMigrations::Unique));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyBlobCol"), QStringLiteral("tinyblob"), TestMigrations::Nullable));
    if (migrator->isDbFeatureAvailable(Firfuorida::Migrator::DefValOnText)) {
        QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("tinytext"), TestMigrations::NoOptions, QStringLiteral("dummer schiss")));
    } else {
        QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("tinytext"), TestMigrations::NoOptions));
    }
    QVERIFY(migrator->rollback());
    QVERIFY(!tableExists(QStringLiteral("tiny")));
}

void TestMySqlMigrations::testMigration()
{
    QVERIFY(m_testmigrator->migrate());
    QVERIFY(tableExists(QStringLiteral("tiny")));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("id"), QStringLiteral("tinyint"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement|TestMigrations::Unsigned));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyIntCol"), QStringLiteral("tinyint"), TestMigrations::Unique));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyBlobCol"), QStringLiteral("tinyblob"), TestMigrations::Nullable));
    if (m_testmigrator->isDbFeatureAvailable(Firfuorida::Migrator::DefValOnBlob)) {
        QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("tinytext"), TestMigrations::NoOptions, QStringLiteral("dummer schiss")));
    } else {
        QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("tinytext"), TestMigrations::NoOptions));
    }
    QVERIFY(tableExists(QStringLiteral("small")));
    QVERIFY(checkColumn(QStringLiteral("small"), QStringLiteral("id"), QStringLiteral("smallint"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement|TestMigrations::Unsigned));
    QVERIFY(tableExists(QStringLiteral("medium")));
    QVERIFY(checkColumn(QStringLiteral("medium"), QStringLiteral("id"), QStringLiteral("int"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement|TestMigrations::Unsigned));
    QVERIFY(tableExists(QStringLiteral("big")));
    QVERIFY(checkColumn(QStringLiteral("big"), QStringLiteral("id"), QStringLiteral("bigint"), TestMigrations::PrimaryKey|TestMigrations::AutoIncrement|TestMigrations::Unsigned));
//    QVERIFY(m_testmigrator->rollback(1));
//    QVERIFY(!tableExists(QStringLiteral("small")));
//    QVERIFY(tableExists(QStringLiteral("tiny")));
//    QVERIFY(m_testmigrator->rollback());
//    QVERIFY(!tableExists(QStringLiteral("small")));
//    QVERIFY(!tableExists(QStringLiteral("tiny")));
}

QTEST_MAIN(TestMySqlMigrations)

#include "testmysqlmigrations.moc"
