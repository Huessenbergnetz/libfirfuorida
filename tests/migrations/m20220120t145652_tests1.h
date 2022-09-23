/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef M20220120T145652_TESTS1_H
#define M20220120T145652_TESTS1_H

#include "../../Firfuorida/migration.h"

class M20220120T145652_Tests1 : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M20220120T145652_Tests1)
public:
    explicit M20220120T145652_Tests1(Firfuorida::Migrator *parent);
    ~M20220120T145652_Tests1() override;

    void up() override;
    void down() override;
};

#endif // M20220120T145652_TESTS1_H

