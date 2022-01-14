/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
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
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        qCCritical(FIR_CORE, "Failed to get database connection \"%s\": %s", qUtf8Printable(connectionName), qUtf8Printable(db.lastError().text()));
        return false;
    }

    Q_Q(Migration);
    q->up();

    const QList<Table *> tables = q->findChildren<Table *>(QString(), Qt::FindDirectChildrenOnly);
    if (tables.empty()) {
        qCWarning(FIR_CORE, "Nothing to do for this migration.");
        return true;
    }

    QSqlQuery query(db);
    for (Table *t : tables) {
        if (t->d_func()->operation == TablePrivate::ExecuteUpFunction) {
            if (!q->executeUp()) {
                qCCritical(FIR_CORE, "Failed to execute custom up function.");
                return false;
            }
        } else {
            if (!query.exec(t->d_func()->queryString())) {
                qCCritical(FIR_CORE, "Failed to execute statement: %s", qUtf8Printable(query.lastError().text()));
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
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        qCCritical(FIR_CORE, "Failed to get database connection \"%s\": %s", qUtf8Printable(connectionName), qUtf8Printable(db.lastError().text()));
        return false;
    }

    Q_Q(Migration);
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
                qCCritical(FIR_CORE, "Failed to execute custom down function.");
                return false;
            }
        } else {
            if (!query.exec(t->d_func()->queryString())) {
                qCCritical(FIR_CORE, "Failed to execute statement: %s", qUtf8Printable(query.lastError().text()));
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

#include "moc_migration.cpp"
