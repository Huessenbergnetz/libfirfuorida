/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef M20220129T115726_FOREIGNKEY1_H
#define M20220129T115726_FOREIGNKEY1_H

#include <Firfuorida/migration.h>

class M20220129T115726_Foreignkey1 : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M20220129T115726_Foreignkey1)
public:
    explicit M20220129T115726_Foreignkey1(Firfuorida::Migrator *parent);
    ~M20220129T115726_Foreignkey1() override;

    void up() override;
    void down() override;
};

#endif // M20220129T115726_FOREIGNKEY1_H

