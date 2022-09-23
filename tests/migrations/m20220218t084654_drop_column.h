/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef M20220218T084654_DROP_COLUMN_H
#define M20220218T084654_DROP_COLUMN_H

#include <Firfuorida/migration.h>

class M20220218T084654_Drop_column : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M20220218T084654_Drop_column)
public:
    explicit M20220218T084654_Drop_column(Firfuorida::Migrator *parent);
    ~M20220218T084654_Drop_column() override;

    void up() override;
    void down() override;
};

#endif // M20220218T084654_DROP_COLUMN_H

