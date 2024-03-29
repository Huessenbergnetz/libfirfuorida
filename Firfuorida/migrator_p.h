/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MIGRATOR_P_H
#define MIGRATOR_P_H

#include "migrator.h"

namespace Firfuorida {

class MigratorPrivate
{
public:
    void setDbType();
    void setDbVersion();

    Error lastError;
    QSqlDatabase db;
    QString connectionName;
    QString migrationsTable;
    QVersionNumber dbVersion;
    Migrator::DatabaseType dbType = Migrator::Invalid;
    Migrator::DatabaseFeatures dbFeatures = Migrator::NoFeatures;
};

}

#endif // MIGRATOR_P_H
