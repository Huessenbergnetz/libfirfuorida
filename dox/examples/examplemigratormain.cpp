/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <QCoreApplication>
#include <QSqlDatabase>
#include <Firfuorida/Migrator>
#include "example.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // add an SQL connection ...

    // create a new Migrator object for the default connection that
    // stores the applied migrations in the "migrations" table
    auto migrator = new Firfuorida::Migrator(QLatin1String(QSqlDatabase::defaultConnection), QStringLiteral("migrations"), &app);

    // add the example migration to the migrator
    new M20190121T174100_Example(migrator);

    // run migrations, return non zero value on error
    if (!migrator->migrate()) {
        return 1;
    }

    // roll back all migrations, return non zero value on error
    if (!migrator->rollback(0)) {
        return 1;
    }

    return 0;
}
