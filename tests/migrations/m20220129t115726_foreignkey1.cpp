/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "m20220129t115726_foreignkey1.h"

M20220129T115726_Foreignkey1::M20220129T115726_Foreignkey1(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220129T115726_Foreignkey1::~M20220129T115726_Foreignkey1()
{

}

void M20220129T115726_Foreignkey1::up()
{
    auto t = create(QStringLiteral("table1"));
    t->increments();
    t->text(QStringLiteral("name"));
    t->date(QStringLiteral("birthday"));
}

void M20220129T115726_Foreignkey1::down()
{
    drop(QStringLiteral("table1"));
}

#include "moc_m20220129t115726_foreignkey1.cpp"

