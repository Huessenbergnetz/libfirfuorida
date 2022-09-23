/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "m20220119t181501_big.h"

M20220119T181501_Big::M20220119T181501_Big(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220119T181501_Big::~M20220119T181501_Big()
{

}

void M20220119T181501_Big::up()
{
    auto t = create(QStringLiteral("big"));
    t->bigIncrements();
    t->bigInteger(QStringLiteral("bigIntCol"));
    t->longBlob(QStringLiteral("longBlobCol"));
    t->longText(QStringLiteral("longTextCol"));
}

void M20220119T181501_Big::down()
{
    drop(QStringLiteral("big"));
}

#include "moc_m20220119t181501_big.cpp"

