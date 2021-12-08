/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_TABLE_P_H
#define FIRFUORIDA_TABLE_P_H

#include "table.h"
#include "migration.h"
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

    QString newName;
    QString engine;
    QString charset;
    QString collation;
    QString raw;
    Table *q_ptr = nullptr;
    TableOperation operation = CreateTable;
    bool temporary = false;
    Q_DECLARE_PUBLIC(Table)
};

}

#endif // FIRFUORIDA_TABLE_P_H
