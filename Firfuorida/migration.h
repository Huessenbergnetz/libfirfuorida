/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_MIGRATION_H
#define FIRFUORIDA_MIGRATION_H

#include "firfuorida_global.h"
#include "table.h"
#include "migrator.h"
#include <QObject>
#include <functional>

namespace Firfuorida {

class MigrationPrivate;

class FIRFUORIDA_LIBRARY Migration : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Migration)
    friend class Migrator;
    const QScopedPointer<MigrationPrivate> dptr;
    Q_DECLARE_PRIVATE_D(dptr, Migration)
public:
    explicit Migration(Migrator *parent);
    ~Migration() override;

protected:
    virtual void up() = 0;
    virtual void down() = 0;
    virtual bool executeUp();
    virtual bool executeDown();
    Table* create(const QString &tableName);
    Table* createTableIfNotExists(const QString &tableName);
    Table* table(const QString &tableName);
    void drop(const QString &tableName);
    void dropIfExists(const QString &tableName);
    void rename(const QString &tableName, const QString &newName);
    void raw(const QString &statement);
    void executeUpFunction();
    void executeDownFunction();
};

}

#endif // FIRFUORIDA_MIGRATION_H
