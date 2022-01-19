#ifndef TESTMIGRATIONS_H
#define TESTMIGRATIONS_H

#include <QObject>
#include "../Firfuorida/migrator.h"

class TestMigrations : public QObject
{
    Q_OBJECT
public:
    explicit TestMigrations(QObject *parent = nullptr);
    ~TestMigrations() override = default;

    enum ColOpt {
        NoOptions = 0,
        Nullable = 1,
        PrimaryKey = 2,
        AutoIncrement = 4,
        Unsigned = 8,
        Unique = 16
    };
    Q_DECLARE_FLAGS(ColOpts, ColOpt)

protected:
    void setDbType(Firfuorida::Migrator::DatabaseType dbType);
    Firfuorida::Migrator::DatabaseType dbType() const;

    void setDbVersion(const QVersionNumber &dbVersion);
    QVersionNumber dbVersion() const;

    void setDbName(const QString &dbName);
    QString dbName() const;

    void setDbRootConn(const QString &dbRootConn);
    QString dbRootConn() const;

private:
    QString m_dbRootConn;
    QString m_dbName;
    QVersionNumber m_dbVersion;
    Firfuorida::Migrator::DatabaseType m_dbType = Firfuorida::Migrator::Invalid;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TestMigrations::ColOpts)

#endif // TESTMIGRATIONS_H
