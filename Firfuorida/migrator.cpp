/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "migrator_p.h"
#include "migration_p.h"
#include <QSqlQuery>
#include <QSqlError>
#include <limits>

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
        qWarning("Invalid/not supported database driver \"%s\" set.", qUtf8Printable(db.driverName()));
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
    const QList<Migration *> migrations = findChildren<Migration *>(QString(), Qt::FindDirectChildrenOnly);
    if (migrations.empty()) {
        qWarning("No migrations added to this migrator.");
        return true;
    }

    Q_D(Migrator);

    d->db = QSqlDatabase::database(d->connectionName);
    if (!d->db.isOpen()) {
        qCritical("Can not open database connection \"%s\": %s", qUtf8Printable(d->connectionName), qUtf8Printable(d->db.lastError().text()));
        return false;
    }

    d->setDbType();

    QSqlQuery query(d->db);
    if (!query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS %1 ("
                                   "migration VARCHAR(255) NOT NULL, "
                                   "applied DATETIME DEFAULT CURRENT_TIMESTAMP, "
                                   "UNIQUE KEY migration (migration)"
                                   ") DEFAULT CHARSET = latin1").arg(d->migrationsTable))) {
        qCritical("Can not create migrations table \"%s\": %s", qUtf8Printable(d->migrationsTable), qUtf8Printable(query.lastError().text()));
        return false;
    }

    QStringList appliedMigrations;
    if (query.exec(QStringLiteral("SELECT migration FROM %1 ORDER BY migration ASC").arg(d->migrationsTable))) {
        while(query.next()) {
            appliedMigrations << query.value(0).toString();
        }
    } else {
        qCritical("Failed to query already applied migrations from the database: %s", qUtf8Printable(query.lastError().text()));
        return false;
    }

    for (Migration *migration : migrations) {
        const QString className = QString::fromLatin1(migration->metaObject()->className());
        if (!appliedMigrations.contains(className)) {
            qInfo("Applying migration %s", migration->metaObject()->className());
            if (migration->d_func()->migrate(d->connectionName)) {
                if (!query.exec(QStringLiteral("INSERT INTO %1 (migration) VALUES ('%2')").arg(d->migrationsTable, className))) {
                    qCritical("Failed to insert applied migration \"%s\" into migration table \"%s\".", migration->metaObject()->className(), qUtf8Printable(d->migrationsTable));
                    return false;
                }
            } else {
                qCritical("Failed to apply migration \"%s\".", migration->metaObject()->className());
                return false;
            }
        }
    }

    return true;
}

bool Migrator::rollback(uint steps)
{
    const QList<Migration *> migrations = findChildren<Migration *>(QString(), Qt::FindDirectChildrenOnly);
    if (migrations.empty()) {
        qWarning("No migrations added to this migrator.");
        return true;
    }

    Q_D(Migrator);

    QSqlDatabase db = QSqlDatabase::database(d->connectionName);
    if (!db.isOpen()) {
        qCritical("Can not open database connection \"%s\": %s", qUtf8Printable(d->connectionName), qUtf8Printable(db.lastError().text()));
        return false;
    }

    d->setDbType();

    QStringList appliedMigrations;
    QSqlQuery query(db);
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
        qCritical("Failed to query already applied migrations from the database: %s", qUtf8Printable(query.lastError().text()));
        return false;
    }

    if (appliedMigrations.empty()) {
        qInfo("%s", "No migrations applied.");
        return true;
    }

    QList<Migration *>::const_reverse_iterator i;
    for (i = migrations.crbegin(); i != migrations.crend(); ++i) {
        Migration *m = *i;
        const QString migrationName = QString::fromLatin1(m->metaObject()->className());
        if (appliedMigrations.contains(migrationName)) {
            qInfo("Rolling back migration %s", m->metaObject()->className());
            if (m->d_func()->rollback(d->connectionName)) {
                if (!query.exec(QStringLiteral("DELETE FROM %1 WHERE migration = '%2'").arg(d->migrationsTable, migrationName))) {
                    qCritical("Failed to remove applied migration \"%s\" from the migration table \"%s\".", m->metaObject()->className(), qUtf8Printable(d->migrationsTable));
                    return false;
                }
            } else {
                qCritical("Failed to rolling back migration \"%s\".", m->metaObject()->className());
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

#include "moc_migrator.cpp"
