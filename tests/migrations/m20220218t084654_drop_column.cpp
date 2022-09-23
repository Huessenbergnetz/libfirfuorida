/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "m20220218t084654_drop_column.h"

M20220218T084654_Drop_column::M20220218T084654_Drop_column(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220218T084654_Drop_column::~M20220218T084654_Drop_column()
{

}

void M20220218T084654_Drop_column::up()
{
    auto t = table(QStringLiteral("tiny"));
    t->integer(QStringLiteral("colToDrop"));
}

void M20220218T084654_Drop_column::down()
{
    if (dbType() == Firfuorida::Migrator::SQLite && dbVersion() < QVersionNumber(3, 35, 0)) {
        // SQLite below version 3.35.0 does not support ALTER TABLE DROP COLUMN
        raw(QStringLiteral("BEGIN TRANSACTION"));

        // create temporary table without column to drop
        auto temp = create(QStringLiteral("tiny_backup"));
        temp->setIsTemporary(true);
        temp->tinyIncrements();
        temp->tinyInteger(QStringLiteral("tinyIntCol"))->unique();
        temp->tinyBlob(QStringLiteral("tinyBlobCol"))->nullable();
        temp->tinyText(QStringLiteral("tinyTextCol"))->defaultValue(QStringLiteral("dummer schiss"));

        // copy current data from original table to backup table
        raw(QStringLiteral("INSERT INTO tiny_backup SELECT id, tinyIntCol, tinyBlobCol, tinyTextCol FROM tiny"));

        // drop original table
        drop(QStringLiteral("tiny"));

        auto nt = create(QStringLiteral("tiny"));
        nt->tinyIncrements();
        nt->tinyInteger(QStringLiteral("tinyIntCol"))->unique();
        nt->tinyBlob(QStringLiteral("tinyBlobCol"))->nullable();
        nt->tinyText(QStringLiteral("tinyTextCol"))->defaultValue(QStringLiteral("dummer schiss"));

        // copy backup data into new table
        raw(QStringLiteral("INSERT INTO tiny SELECT id, tinyIntCol, tinyBlobCol, tinyTextCol FROM tiny_backup"));

        // drop backup table
        drop(QStringLiteral("tiny_backup"));

        raw(QStringLiteral("COMMIT"));
    } else {
        auto t = table(QStringLiteral("tiny"));
        t->dropColumn(QStringLiteral("colToDrop"));
    }
}

#include "moc_m20220218t084654_drop_column.cpp"

