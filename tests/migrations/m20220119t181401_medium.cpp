/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "m20220119t181401_medium.h"

M20220119T181401_Medium::M20220119T181401_Medium(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220119T181401_Medium::~M20220119T181401_Medium()
{

}

void M20220119T181401_Medium::up()
{
    auto t = create(QStringLiteral("medium"));
    t->increments();
    t->integer(QStringLiteral("intCol"));
    t->mediumBlob(QStringLiteral("mediumBlobCol"));
    t->mediumText(QStringLiteral("mediumTextCol"));
}

void M20220119T181401_Medium::down()
{
    drop(QStringLiteral("medium"));
}

#include "moc_m20220119t181401_medium.cpp"

