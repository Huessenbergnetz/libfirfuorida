/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_MIGRATION_P_H
#define FIRFUORIDA_MIGRATION_P_H

#include "migration.h"

namespace Firfuorida {

class MigrationPrivate {
public:
    bool migrate(const QString &connectionName);
    bool rollback(const QString &connectionName);

    Migration *q_ptr = nullptr;
    Error lastError;
    Q_DECLARE_PUBLIC(Migration)
};

}

#endif // FIRFUORIDA_MIGRATION_P_H
