/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "migrator_p.h"
#include "migration_p.h"
#include <QSqlQuery>
#include <QSqlError>
#include <limits>
#include "logging.h"

Q_LOGGING_CATEGORY(FIR_CORE, "libfirfuorida.core")

using namespace Firfuorida;

void MigratorPrivate::setDbType()
{
    if (db.driverName() == QLatin1String("QDB2")) {
        dbType = Migrator::DB2;
    } else if (db.driverName() == QLatin1String("QIBASE")) {
        dbType = Migrator::InterBase;
    } else if (db.driverName() == QLatin1String("QMYSQL")) {
        dbType = Migrator::MySQL;
    } else if (db.driverName() == QLatin1String("MARIADB")) {
        dbType = Migrator::MariaDB;
    } else if (db.driverName() == QLatin1String("ODBC")) {
        dbType = Migrator::ODBC;
    } else if (db.driverName() == QLatin1String("QOCI")) {
        dbType = Migrator::OCI;
    } else if (db.driverName() == QLatin1String("QPSQL")) {
        dbType = Migrator::PSQL;
    } else if (db.driverName() == QLatin1String("QSQLITE")) {
        dbType = Migrator::SQLite;
    } else {
        qCWarning(FIR_CORE, "Invalid/not supported database driver \"%s\" set.", qUtf8Printable(db.driverName()));
    }
}

void MigratorPrivate::setDbVersion()
{
    QSqlQuery q(db);
    if (dbType == Migrator::MySQL || dbType == Migrator::MariaDB) {
        if (q.exec(QStringLiteral("SHOW VARIABLES WHERE Variable_name = 'version'"))) {
            if (q.next()) {
                const QString version = q.value(1).toString();
                int suffixIndex = -1;
                dbVersion = QVersionNumber::fromString(version, &suffixIndex);
                if (!dbVersion.isNull()) {
                    if (suffixIndex > -1) {
                        if (version.mid(suffixIndex).contains(QLatin1String("MariaDB"), Qt::CaseInsensitive)) {
                            dbType = Migrator::MariaDB;
                        }
                    }
                } else {
                    qCWarning(FIR_CORE, "Can not find valid database version information in \"%s\".", qUtf8Printable(version));
                }
            } else {
                qCCritical(FIR_CORE, "%s", "Can not find database version information.");
            }
        } else {
            qCCritical(FIR_CORE, "Failed to execute query to determine MySQL/MariaDB database version: %s", qUtf8Printable(q.lastError().text()));
        }
    } else if (dbType == Migrator::SQLite) {
        if (q.exec(QStringLiteral("SELECT sqlite_version()"))) {
            if (q.next()) {
                const QString version = q.value(0).toString();
                dbVersion = QVersionNumber::fromString(version);
            } else {
                qCCritical(FIR_CORE, "%s", "Can not find database version information.");
            }
        } else {
            qCCritical(FIR_CORE, "Failed to execute query to determine SQLite database version: %s", qUtf8Printable(q.lastError().text()));
        }
    }
}

Migrator::Migrator(QObject *parent) :
    QObject(parent), dptr(new MigratorPrivate)
{
    Q_D(Migrator);
    d->connectionName = QLatin1String(QSqlDatabase::defaultConnection);
    d->migrationsTable = QStringLiteral("migrations");
}

Migrator::Migrator(const QString &connectionName, const QString &migrationsTable, QObject *parent) : QObject(parent), dptr(new MigratorPrivate)
{
    Q_D(Migrator);
    d->connectionName = connectionName;
    d->migrationsTable = migrationsTable;
}

Migrator::~Migrator()
{

}

bool Migrator::initDatabase()
{
    Q_D(Migrator);

    if (!d->db.isOpen()) {
        d->db = QSqlDatabase::database(d->connectionName);
        if (!d->db.isOpen()) {
            d->lastError = Error(d->db.lastError(), QStringLiteral("Can not open database connection \"%1\":").arg(d->connectionName));
            qCCritical(FIR_CORE) << d->lastError;
            return false;
        }
    }

    d->setDbType();
    d->setDbVersion();

    switch (d->dbType) {
    case DB2:
    case InterBase:
        break;
    case MySQL:
    {
        d->dbFeatures |= GeometryTypes;
        if (d->dbVersion >= QVersionNumber(5,7,8)) {
            d->dbFeatures |= JSONTypes;
        }
        if (d->dbVersion >= QVersionNumber(8,0,13)) {
            d->dbFeatures |= DefValOnText;
            d->dbFeatures |= DefValOnBlob;
            d->dbFeatures |= DefValOnGeometry;
        }
        d->dbFeatures |= ForeignKeys;
        d->dbFeatures |= CommentsOnColumns;
        d->dbFeatures |= CommentsOnTables;
        d->dbFeatures |= SetType;
        d->dbFeatures |= EnumType;
        d->dbFeatures |= UnsignedInteger;
        d->dbFeatures |= CharsetOnColumn;
    }
        break;
    case MariaDB:
    {
        if (d->dbVersion >= QVersionNumber(10,2,1)) {
            d->dbFeatures |= DefValOnText;
            d->dbFeatures |= DefValOnBlob;
        }
        d->dbFeatures |= ForeignKeys;
        d->dbFeatures |= CommentsOnColumns;
        d->dbFeatures |= CommentsOnTables;
        d->dbFeatures |= SetType;
        d->dbFeatures |= EnumType;
        d->dbFeatures |= UnsignedInteger;
        d->dbFeatures |= CharsetOnColumn;
    }
        break;
    case ODBC:
    case OCI:
        break;
    case PSQL:
    {
        d->dbFeatures |= DefValOnText;
        d->dbFeatures |= DefValOnBlob;
        d->dbFeatures |= DefValOnGeometry;
        d->dbFeatures |= JSONTypes;
        d->dbFeatures |= GeometryTypes;
        d->dbFeatures |= XMLType;
        d->dbFeatures |= NetworkAddressTypes;
        d->dbFeatures |= MonetaryTypes;
        d->dbFeatures |= ForeignKeys;
        d->dbFeatures |= CommentsOnColumns;
        d->dbFeatures |= CommentsOnTables;
        d->dbFeatures |= SetType;
        d->dbFeatures |= EnumType;
    }
        break;
    case SQLite:
    {
        d->dbFeatures |= DefValOnText;
        d->dbFeatures |= DefValOnBlob;
        if (d->dbVersion >= QVersionNumber(3,6,19)) {
            d->dbFeatures |= ForeignKeys;
        }
    }
        break;
    default:
        break;
    }

    return true;
}

Migrator::DatabaseType Migrator::dbType() const
{
    Q_D(const Migrator);
    return d->dbType;
}

QString Migrator::dbTypeToStr() const
{
    Q_D(const Migrator);
    switch(d->dbType) {
    case DB2:
        return QStringLiteral("IBM DB2");
    case InterBase:
        return QStringLiteral("Borland InterBase");
    case MySQL:
        return QStringLiteral("MySQL");
    case MariaDB:
        return QStringLiteral("MariaDB");
    case ODBC:
        return QStringLiteral("Open Database Connectivity (ODBC)");
    case OCI:
        return QStringLiteral("Oracle Call Interface (OCI)");
    case PSQL:
        return QStringLiteral("PostgreSQL");
    case SQLite:
        return QStringLiteral("SQLite");
    default:
        return QStringLiteral("Invalid");
    }
}

QVersionNumber Migrator::dbVersion() const
{
    Q_D(const Migrator);
    return d->dbVersion;
}

Migrator::DatabaseFeatures Migrator::dbFeatures() const
{
    Q_D(const Migrator);
    return d->dbFeatures;
}

bool Migrator::isDbFeatureAvailable(DatabaseFeatures dbFeatures) const
{
    Q_D(const Migrator);
    return (d->dbFeatures & static_cast<int>(dbFeatures)) != 0;
}

QString Migrator::connectionName() const
{
    Q_D(const Migrator);
    return d->connectionName;
}

QString Migrator::migrationsTable() const
{
    Q_D(const Migrator);
    return d->migrationsTable;
}

bool Migrator::migrate()
{
    Q_D(Migrator);

    d->lastError = Error();

    const QList<Migration *> migrations = findChildren<Migration *>(QString(), Qt::FindDirectChildrenOnly);
    if (migrations.empty()) {
        qCWarning(FIR_CORE, "No migrations added to this migrator.");
        return true;
    }

    if (!initDatabase()) {
        return false;
    }

    qCInfo(FIR_CORE, "Start database migrations on %s database version %s", qUtf8Printable(dbTypeToStr()), qUtf8Printable(d->dbVersion.toString()));

    QSqlQuery query(d->db);
    if (dbType() == Migrator::SQLite) {
        if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS %1 ("
                                       "migration TEXT NOT NULL UNIQUE, "
                                       "applied NUMERIC DEFAULT CURRENT_TIMESTAMP)").arg(d->migrationsTable))) {
            d->lastError = Error(query.lastError(), QStringLiteral("Can not create migrations table \"%s\":").arg(d->migrationsTable));
            qCCritical(FIR_CORE) << d->lastError;
            return false;
        }
    } else {
        if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS %1 ("
                                       "migration VARCHAR(255) NOT NULL, "
                                       "applied DATETIME DEFAULT CURRENT_TIMESTAMP, "
                                       "UNIQUE KEY migration (migration)"
                                       ") DEFAULT CHARSET = latin1").arg(d->migrationsTable))) {
            d->lastError = Error(query.lastError(), QStringLiteral("Can not create migrations table \"%s\":").arg(d->migrationsTable));
            qCCritical(FIR_CORE) << d->lastError;
            return false;
        }
    }

    QStringList appliedMigrations;
    if (query.exec(QStringLiteral("SELECT migration FROM %1 ORDER BY migration ASC").arg(d->migrationsTable))) {
        while(query.next()) {
            appliedMigrations << query.value(0).toString();
        }
    } else {
        d->lastError = Error(query.lastError(), QStringLiteral("Failed to query already applied migrations from the database:"));
        qCCritical(FIR_CORE) << d->lastError;
        return false;
    }

    for (Migration *migration : migrations) {
        const QString className = QString::fromLatin1(migration->metaObject()->className());
        if (!appliedMigrations.contains(className)) {
            qCInfo(FIR_CORE, "Applying migration %s", migration->metaObject()->className());
            if (migration->d_func()->migrate(d->connectionName)) {
                if (!query.exec(QStringLiteral("INSERT INTO %1 (migration) VALUES ('%2')").arg(d->migrationsTable, className))) {
                    d->lastError = Error(query.lastError(), QStringLiteral("Failed to insert applied migration \"%s\" into migration table \"%s\":").arg(QString::fromLatin1(migration->metaObject()->className()), d->migrationsTable));
                    qCCritical(FIR_CORE) << d->lastError;
                    return false;
                }
            } else {
                d->lastError = migration->lastError();
                return false;
            }
        }
    }

    return true;
}

bool Migrator::rollback(uint steps)
{
    Q_D(Migrator);

    d->lastError = Error();

    const QList<Migration *> migrations = findChildren<Migration *>(QString(), Qt::FindDirectChildrenOnly);
    if (migrations.empty()) {
        qCWarning(FIR_CORE, "No migrations added to this migrator.");
        return true;
    }

    if (!initDatabase()) {
        return false;
    }

    qCInfo(FIR_CORE, "Start rolling back database migrations on %s database version %s", qUtf8Printable(dbTypeToStr()), qUtf8Printable(d->dbVersion.toString()));

    QStringList appliedMigrations;
    QSqlQuery query(d->db);
    QString qs = QStringLiteral("SELECT migration FROM %1 ORDER BY migration DESC").arg(d->migrationsTable);
    if (steps > 0) {
        qs += QStringLiteral(" LIMIT %1").arg(steps);
    } else {
        qs += QStringLiteral(" LIMIT 1");
    }

    if (query.exec(qs)) {
        while(query.next()) {
            appliedMigrations << query.value(0).toString();
        }
    } else {
        d->lastError = Error(query.lastError(), QStringLiteral("Failed to query already applied migrations from the database:"));
        qCCritical(FIR_CORE) << d->lastError;
        return false;
    }

    if (appliedMigrations.empty()) {
        qCInfo(FIR_CORE, "%s", "No migrations applied.");
        return true;
    }

    QList<Migration *>::const_reverse_iterator i;
    for (i = migrations.crbegin(); i != migrations.crend(); ++i) {
        Migration *m = *i;
        const QString migrationName = QString::fromLatin1(m->metaObject()->className());
        if (appliedMigrations.contains(migrationName)) {
            qCInfo(FIR_CORE, "Rolling back migration %s", m->metaObject()->className());
            if (m->d_func()->rollback(d->connectionName)) {
                if (!query.exec(QStringLiteral("DELETE FROM %1 WHERE migration = '%2'").arg(d->migrationsTable, migrationName))) {
                    d->lastError = Error(query.lastError(), QStringLiteral("Failed to remove applied migration \"%s\" from the migrations table \"%s\":").arg(QString::fromLatin1(m->metaObject()->className()), d->migrationsTable));
                    qCCritical(FIR_CORE) << d->lastError;
                    return false;
                }
            } else {
                d->lastError = m->lastError();
                return false;
            }
        }
    }

    return true;
}

bool Migrator::reset()
{
    return rollback(std::numeric_limits<uint>::max());
}

bool Migrator::refresh(uint steps)
{
    if (steps < 1) {
        if (!rollback(std::numeric_limits<uint>::max())) {
            return false;
        }
    } else {
        if (!rollback(steps)) {
            return false;
        }
    }

    return migrate();
}

Error Migrator::lastError() const
{
    Q_D(const Migrator);
    return d->lastError;
}

QVersionNumber Firfuorida::version()
{
    return QVersionNumber::fromString(QStringLiteral(FIRFUORIDA_VERSION));
}

#include "moc_migrator.cpp"
