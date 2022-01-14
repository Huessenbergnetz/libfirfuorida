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
 * <h2>Example</h2>
 * example.cpp
 * \include examplemigration.cpp
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

    /*!
     * \brief Enables or disables the auto increment feature for this column.
     */
    Column* autoIncrement(bool autoIncrement = true);
    /*!
     * \brief Sets the column to signed or unsigned.
     */
    Column* unSigned(bool unSigned = true);
    /*!
     * \brief Sets the character set used for this column.
     */
    Column* charset(const QString &charset);
    /*!
     * \brief Sets the collation used for this column.
     */
    Column* collation(const QString &collation);
    /*!
     * \brief Sets the default value for this column.
     */
    Column* defaultValue(const QVariant &defVal);
    /*!
     * \brief Sets the columns to nullable or not.
     */
    Column* nullable(bool isNullable = true);
    Column* useCurrent(bool useCurrent = true);
    /*!
     * \brief Makes the column a primary key column or not.
     */
    Column* primary(bool primary = true);
    /*!
     * \brief Makes the column a unique key column or not.
     */
    Column* unique(bool unique = true);
    /*!
     * \brief Adds the onDelete action to a foreign key column.
     */
    Column* onDelete(const QString &referenceOption);
    /*!
     * \brief Adds the onUpdate action to a foreign key column.
     */
    Column* onUpdate(const QString &referenceOption);
    /*!
     * \brief Add a \a comment to the column.
     * \note The maximum size for a column comment on MySQL/MariaDB is \c 1024 characters.
     */
    Column* comment(const QString &comment);
    /*!
     * \brief Put/move this column after \a otherColumn.
     */
    void after(const QString &otherColumn);
    /*!
     * \brief Add/move this column to the beginning as first column.
     */
    void first();
    /*!
     * \brief Mark this column as a column to change instead of creating it.
     */
    void change();
};

}

#endif // FIRFUORIDA_COLUMN_H
