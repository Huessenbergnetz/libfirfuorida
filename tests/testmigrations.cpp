#include "testmigrations.h"

TestMigrations::TestMigrations(QObject *parent)
    : QObject{parent}
{

}

void TestMigrations::setDbType(Firfuorida::Migrator::DatabaseType dbType)
{
    m_dbType = dbType;
}


Firfuorida::Migrator::DatabaseType TestMigrations::dbType() const
{
    return m_dbType;
}

void TestMigrations::setDbVersion(const QVersionNumber &dbVersion)
{
    m_dbVersion = dbVersion;
}

QVersionNumber TestMigrations::dbVersion() const
{
    return m_dbVersion;
}

void TestMigrations::setDbName(const QString &dbName)
{
    m_dbName = dbName;
}

QString TestMigrations::dbName() const
{
    return m_dbName;
}

void TestMigrations::setDbRootConn(const QString &dbRootConn)
{
    m_dbRootConn = dbRootConn;
}

QString TestMigrations::dbRootConn() const
{
    return m_dbRootConn;
}

QString TestMigrations::currentTimeStampDefVal() const
{
    if (dbType() == Firfuorida::Migrator::MySQL) {
        return QStringLiteral("CURRENT_TIMESTAMP");
    } else {
        return QStringLiteral("current_timestamp()");
    }
}
