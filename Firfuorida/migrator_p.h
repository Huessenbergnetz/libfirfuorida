/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MIGRATOR_P_H
#define MIGRATOR_P_H

#include "migrator.h"
#include <QVersionNumber>

namespace Firfuorida {

class MigratorPrivate
{
public:
    void setDbType();
    QString dbTypeToStr() const;
    void setDbVersion();

    QSqlDatabase db;
    QString connectionName;
    QString migrationsTable;
    QVersionNumber dbVersion;
    Migrator::DatabaseType dbType = Migrator::Invalid;
};

}

#endif // MIGRATOR_P_H
