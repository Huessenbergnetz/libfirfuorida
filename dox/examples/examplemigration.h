/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include <Firfuorida/Migration>

class M20190121T174100_Example : public Firfuorida::Migration
{
    Q_OBJECT
public:
    explicit M20190121T174100_Example(Firfuorida::Migrator *parent);
    ~M20190121T174100_Example() override;

protected:
    void up() override;
    void down() override;
}
