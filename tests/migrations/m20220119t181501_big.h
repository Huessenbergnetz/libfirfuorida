/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef M20220119T181501_BIG_H
#define M20220119T181501_BIG_H

#include "../../Firfuorida/migration.h"

class M20220119T181501_Big : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M20220119T181501_Big)
public:
    explicit M20220119T181501_Big(Firfuorida::Migrator *parent);
    ~M20220119T181501_Big() override;

    void up() override;
    void down() override;
};

#endif // M20220119T181501_BIG_H

