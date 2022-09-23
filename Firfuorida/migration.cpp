/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "migration_p.h"
#include "table_p.h"
#include "table.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "logging.h"

using namespace Firfuorida;

bool MigrationPrivate::migrate(const QString &connectionName)
{
    lastError = Error();

    Q_Q(Migration);

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        lastError = Error(db.lastError(), QStringLiteral("Failed to open database connection \"%1\" to perform migration \"%2\".").arg(connectionName, QString::fromLatin1(q->metaObject()->className())));
        qCCritical(FIR_CORE) << lastError;
        return false;
    }

    q->up();

    const QList<Table *> tables = q->findChildren<Table *>(QString(), Qt::FindDirectChildrenOnly);
    if (tables.empty()) {
        qCWarning(FIR_CORE, "Nothing to do for migration \"%s\".", qUtf8Printable(QString::fromLatin1(q->metaObject()->className())));
        return true;
    }

    QSqlQuery query(db);
    for (Table *t : tables) {
        if (t->d_func()->operation == TablePrivate::ExecuteUpFunction) {
            if (!q->executeUp()) {
                lastError = Error(Error::InternalError, QStringLiteral("Failed to execute custom up function for migration \"%1\".").arg(QString::fromLatin1(q->metaObject()->className())));
                qCCritical(FIR_CORE) << lastError;
                return false;
            }
        } else {
            if (!query.exec(t->d_func()->queryString())) {
                lastError = Error(query.lastError(), QStringLiteral("Failed to execute SQL query for migration \"%1\".").arg(QString::fromLatin1(q->metaObject()->className())));
                qCCritical(FIR_CORE) << lastError;
                qCCritical(FIR_CORE, "Failed query: %s", qUtf8Printable(query.lastQuery()));
                return false;
            }
        }
    }

    qDeleteAll(tables);

    return true;
}

bool MigrationPrivate::rollback(const QString &connectionName)
{
    lastError = Error();

    Q_Q(Migration);

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        lastError = Error(db.lastError(), QStringLiteral("Failed to open database connection \"%1\" to perform rollback of migration \"%2\".").arg(connectionName, QString::fromLatin1(q->metaObject()->className())));
        qCCritical(FIR_CORE) << lastError;
        return false;
    }

    q->down();

    const QList<Table *> tables = q->findChildren<Table *>(QString(), Qt::FindDirectChildrenOnly);
    if (tables.empty()) {
        qCWarning(FIR_CORE, "Nothing to do for this migration.");
        return true;
    }

    QSqlQuery query(db);
    for (Table *t : tables) {
        if (t->d_func()->operation == TablePrivate::ExecuteDownFunction) {
            if (!q->executeDown()) {
                lastError = Error(Error::InternalError, QStringLiteral("Failed to execute custom down function for migration \"%1\".").arg(QString::fromLatin1(q->metaObject()->className())));
                qCCritical(FIR_CORE) << lastError;
                return false;
            }
        } else {
            if (!query.exec(t->d_func()->queryString())) {
                lastError = Error(query.lastError(), QStringLiteral("Failed to execute SQL query for rolling back \"%1\".").arg(QString::fromLatin1(q->metaObject()->className())));
                qCCritical(FIR_CORE) << lastError;
                qCCritical(FIR_CORE, "Failed query: %s", qUtf8Printable(query.lastQuery()));
                return false;
            }
        }
    }

    qDeleteAll(tables);

    return true;
}

Migration::Migration(Migrator *parent) : QObject(parent), dptr(new MigrationPrivate)
{
    Q_D(Migration);
    d->q_ptr = this;
}

Migration::~Migration()
{

}

Table *Migration::create(const QString &tableName)
{
    Q_ASSERT_X(!tableName.trimmed().isEmpty(), "creating table", "empty table name");
    auto t = new Table(this);
    t->setObjectName(tableName.trimmed());
    t->dptr->operation = TablePrivate::CreateTable;
    return t;
}

Table *Migration::createTableIfNotExists(const QString &tableName)
{
    Q_ASSERT_X(!tableName.trimmed().isEmpty(), "creating table", "empty table name");
    auto t = new Table(this);
    t->setObjectName(tableName.trimmed());
    t->dptr->operation = TablePrivate::CreateTableIfNotExists;
    return t;
}

Table *Migration::table(const QString &tableName)
{
    Q_ASSERT_X(!tableName.trimmed().isEmpty(), "modifying table", "empty table name");
    auto t = new Table(this);
    t->setObjectName(tableName.trimmed());
    t->dptr->operation = TablePrivate::ModifyTable;
    return t;
}

void Migration::drop(const QString &tableName)
{
    Q_ASSERT_X(!tableName.trimmed().isEmpty(), "dropping table", "empty table name");
    auto t = new Table(this);
    t->setObjectName(tableName.trimmed());
    t->dptr->operation = TablePrivate::DropTable;
}

void Migration::dropIfExists(const QString &tableName)
{
    Q_ASSERT_X(!tableName.trimmed().isEmpty(), "dropping table", "empty table name");
    auto t = new Table(this);
    t->setObjectName(tableName.trimmed());
    t->dptr->operation = TablePrivate::DropTableIfExists;
}

void Migration::rename(const QString &tableName, const QString &newName)
{
    Q_ASSERT_X(!tableName.trimmed().isEmpty(), "renaming table", "empty table name");
    Q_ASSERT_X(!newName.trimmed().isEmpty(), "renaming table", "empty new table name");
    auto t = new Table(this);
    t->setObjectName(tableName.trimmed());
    t->dptr->operation = TablePrivate::RenameTable;
    t->dptr->newName = newName;
}

void Migration::raw(const QString &statement)
{
    auto t = new Table(this);
    t->d_func()->operation = TablePrivate::Raw;
    t->d_func()->raw = statement;
}

void Migration::executeUpFunction()
{
    auto t = new Table(this);
    t->d_func()->operation = TablePrivate::ExecuteUpFunction;
}

bool Migration::executeUp()
{
    return false;
}

void Migration::executeDownFunction()
{
    auto t = new Table(this);
    t->d_func()->operation = TablePrivate::ExecuteDownFunction;
}

bool Migration::executeDown()
{
    return false;
}

Migrator::DatabaseType Migration::dbType() const
{
    return qobject_cast<Migrator*>(parent())->dbType();
}

QString Migration::dbTypeToStr() const
{
    return qobject_cast<Migrator*>(parent())->dbTypeToStr();
}

QVersionNumber Migration::dbVersion() const
{
    return qobject_cast<Migrator*>(parent())->dbVersion();
}

Migrator::DatabaseFeatures Migration::dbFeatures() const
{
    return qobject_cast<Migrator*>(parent())->dbFeatures();
}

bool Migration::isDbFeatureAvailable(Migrator::DatabaseFeatures dbFeatures) const
{
    return qobject_cast<Migrator*>(parent())->isDbFeatureAvailable(dbFeatures);
}

Error Migration::lastError() const
{
    Q_D(const Migration);
    return d->lastError;
}

#include "moc_migration.cpp"
