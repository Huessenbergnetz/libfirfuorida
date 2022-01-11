/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MIGRATOR_H
#define MIGRATOR_H

#include "firfuorida_global.h"
#include <QObject>
#include <QSqlDatabase>

namespace Firfuorida {

class MigratorPrivate;

/*!
 * \brief Manages multiple migrations.
 *
 * The %Migrator class manages multiple Migration objects that are children of the
 * %Migrator class.
 *
 * \headerfile "" <Firfuorida/Migrator>
 */
class FIRFUORIDA_LIBRARY Migrator : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Migrator)
    const QScopedPointer<MigratorPrivate> dptr;
    Q_DECLARE_PRIVATE_D(dptr, Migrator)
public:
    explicit Migrator(const QString &connectionName = QLatin1String(QSqlDatabase::defaultConnection), const QString &migrationsTable = QStringLiteral("migrations"), QObject *parent = nullptr);
    ~Migrator();

    bool migrate();
    bool rollback(uint steps = 1);
    bool reset();
    bool refresh(uint steps = 0);
};

}

#endif // MIGRATOR_H
