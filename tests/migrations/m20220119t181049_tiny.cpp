/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "m20220119t181049_tiny.h"

M20220119t181049_Tiny::M20220119t181049_Tiny(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220119t181049_Tiny::~M20220119t181049_Tiny()
{

}

void M20220119t181049_Tiny::up()
{
    auto t = create(QStringLiteral("tiny"));
    t->tinyIncrements();
    t->tinyInteger(QStringLiteral("tinyIntCol"))->unique();
    t->tinyBlob(QStringLiteral("tinyBlobCol"))->nullable();
    t->tinyText(QStringLiteral("tinyTextCol"))->defaultValue(QStringLiteral("dummer schiss"));
}

void M20220119t181049_Tiny::down()
{
    drop(QStringLiteral("tiny"));
}

#include "moc_m20220119t181049_tiny.cpp"

