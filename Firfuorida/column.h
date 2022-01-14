/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_COLUMN_H
#define FIRFUORIDA_COLUMN_H

#include "firfuorida_global.h"
#include <QObject>
#include <QVariant>
#include <QtGlobal>

namespace Firfuorida {

class ColumnPrivate;

/*!
 * \brief Contains information about a single column, either to creaste or to modify.
 *
 * The %Column object can only be created by functions of the Table class.
 *
 * \headerfile "" <Firfuorida/Column>
 */
class FIRFUORIDA_LIBRARY Column : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Column)
    friend class Table;
    friend class TablePrivate;
    const QScopedPointer<ColumnPrivate> dptr;
    Q_DECLARE_PRIVATE_D(dptr, Column)
    explicit Column(QObject *parent);
public:
    /*!
     * \brief Deconstructs the %Column object.
     */
    ~Column() override;

    Column* autoIncrement();
    Column* unSigned();
    Column* charset(const QString &charset);
    Column* collation(const QString &collation);
    Column* defaultValue(const QVariant &defVal);
    Column* nullable(bool isNullable = true);
    Column* useCurrent();
    Column* primary();
    Column* unique();
    Column* onDelete(const QString &referenceOption);
    Column* onUpdate(const QString &referenceOption);
    Column* comment(const QString &comment);
    void after(const QString &columnName);
    void first();
    void change();
};

}

#endif // FIRFUORIDA_COLUMN_H
