/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_TABLE_P_H
#define FIRFUORIDA_TABLE_P_H

#include "table.h"
#include "migration.h"
#include "migrator.h"
#include <functional>

namespace Firfuorida {

class TablePrivate {
public:
    enum TableOperation : quint8 {
        CreateTable,
        CreateTableIfNotExists,
        DropTable,
        DropTableIfExists,
        ModifyTable,
        RenameTable,
        Raw,
        ExecuteUpFunction,
        ExecuteDownFunction
    };

    QString queryString() const;

    Migrator::DatabaseType dbType() const;
    QString dbTypeToStr() const;
    QVersionNumber dbVersion() const;
    Migrator::DatabaseFeatures dbFeatures() const;
    bool isDbFeatureAvailable(Migrator::DatabaseFeatures dbFeatures) const;

    QString newName;
    QString engine;
    QString charset;
    QString collation;
    QString raw;
    QString comment;
    Table *q_ptr = nullptr;
    TableOperation operation = CreateTable;
    bool temporary = false;
    Q_DECLARE_PUBLIC(Table)
};

}

#endif // FIRFUORIDA_TABLE_P_H
