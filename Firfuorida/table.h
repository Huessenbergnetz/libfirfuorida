/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_TABLE_H
#define FIRFUORIDA_TABLE_H

#include "firfuorida_global.h"
#include "column.h"
#include <QObject>

namespace Firfuorida {

class TablePrivate;
class Migration;

/*!
 * \brief Contains information about a single table, either to create or to modify.
 *
 * The %Table object can only be created by functions of the Migration class.
 *
 * <h2>Example</h2>
 * example.h
 * \include examplemigration.h
 *
 * example.cpp
 * \include examplemigration.cpp
 *
 * \headerfile "" <Firfuorida/Table>
 */
class FIRFUORIDA_LIBRARY Table : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Table)
    friend class Migration;
    friend class MigrationPrivate;
    const QScopedPointer<TablePrivate> dptr;
    F_DECLARE_PRIVATE_D(dptr, Table)
    explicit Table(Migration *parent);
public:
    /*!
     * \brief Deconstructs the %Table object.
     */
    ~Table() override;

    /*!
     * \brief Set the used \a engine when creating a new table or change it for an existing table.
     */
    void setEngine(const QString &engine);
    /*!
     * \brief Set the default \a charset when creating a new table or change it for an existing table.
     */
    void setCharset(const QString &charset);
    /*!
     * \brief Set the \a collation when creating a new table or change it for an existing table.
     */
    void setCollation(const QString &collation);
    /*!
     * \brief Sets a \a comment when creating a new table of change it for an existing table.
     * \note The maximum size for a column comment on MySQL/MariaDB is \c 2048 characters.
     */
    void setComment(const QString &comment);
    /*!
     * \brief Marks the table as temporary if \a isTemporary is set to \c true.
     */
    void setIsTemporary(bool isTemporary = true);

    /*!
     * \brief Creates/modifies an unsigned tiny integer primary key column that auto increments with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName TINYINT UNSIGNED AUTO_INCREMENT PRIMARY KEY
     * \endcode
     */
    Column* tinyIncrements(const QString &columnName = QStringLiteral("id"));
    /*!
     * \brief Creates/modifies a signed tiny integer column with given \a columnName and \a displayWidth
     * \a displayWidth will only be set if greate than \c 0. Use Column::unsigned() on the returned Column object
     * to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName TINYINT(displayWidth}
     * \endcode
     */
    Column* tinyInteger(const QString &columnName, uint displayWidth = 0);
    /*!
     * \brief Creates/modifies an unsigned small integer primary key column that auto increments with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName SMALLINT UNSIGNED AUTO_INCREMENT PRIMARY KEY
     * \endcode
     */
    Column* smallIncrements(const QString &columnName = QStringLiteral("id"));
    /*!
     * \brief Creates/modifies a signed small integer column with given \a columnName and \a displayWidth and returns a pointer to the Column object.
     * \a displayWidth will only be set if greate than \c 0. Use Column::unsigned() on the returned Column object
     * to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName SMALLINT(displayWidth}
     * \endcode
     */
    Column* smallInteger(const QString &columnName, uint displayWidth = 0);
    /*!
     * \brief Creates/modifies an unsigned medium integer primary key column that auto increments with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName MEDIUMINT UNSIGNED AUTO_INCREMENT PRIMARY KEY
     * \endcode
     */
    Column* mediumIncrements(const QString &columnName = QStringLiteral("id"));
    /*!
     * \brief Creates/modifies a signed medium integer column with given \a columnName and \a displayWidth and returns a pointer to the Column object.
     * \a displayWidth will only be set if greate than \c 0. Use Column::unsigned() on the returned Column object
     * to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName MEDIUMINT(displayWidth}
     * \endcode
     */
    Column* mediumInteger(const QString &columnName, uint displayWidth = 0);
    /*!
     * \brief Creates/modifies an unsigned integer primary key column that auto increments with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName INT UNSIGNED AUTO_INCREMENT PRIMARY KEY
     * \endcode
     */
    Column* increments(const QString &columnName = QStringLiteral("id"));
    /*!
     * \brief Creates/modifies a signed integer column with given \a columnName and \a displayWidth and returns a pointer to the Column object.
     * \a displayWidth will only be set if greate than \c 0. Use Column::unsigned() on the returned Column object
     * to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName INT(displayWidth}
     * \endcode
     */
    Column* integer(const QString &columnName, uint displayWidth = 0);
    /*!
     * \brief Creates/modifies an unsigned big integer primary key column that auto increments with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY
     * \endcode
     */
    Column* bigIncrements(const QString &columnName = QStringLiteral("id"));
    /*!
     * \brief Creates/modifies a signed big integer column with given \a columnName and \a displayWidth and returns a pointer to the Column object.
     * \a displayWidth will only be set if greate than \c 0. Use Column::unsigned() on the returned Column object
     * to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName BIGINT(displayWidth}
     * \endcode
     */
    Column* bigInteger(const QString &columnName, uint displayWidth = 0);
    /*!
     * \brief Creates/modifies a signed  decimal column with given \a columnName, \a precision and \a scale and returns a pointer to the Column object.
     * Use Column::unsigned() on the returned Column object to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName DECIMAL(precision, scale)
     * \endcode
     */
    Column* decimal(const QString &columnName, uint precision, uint scale);
    /*!
     * \brief Creates/modifies a signed numeric column with given \a columnName, \a precision and \a scale and returns a pointer to the Column object.
     * Use Column::unsigned() on the returned Column object to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName NUMERIC(precision, scale)
     * \endcode
     */
    Column* numeric(const QString &columnName, uint precision, uint scale);
    /*!
     * \brief Creates/modifies a signed float column with given \a columnName, \a precision and \a scale and returns a pointer to the Column object.
     * Use Column::unsigned() on the returned Column object to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName FLOAT(precision, scale)
     * \endcode
     */
    Column* floatCol(const QString &columnName, uint precision, uint scale);
    /*!
     * \brief Creates/modifies a signed double column with given \a columnName, \a precision and \a scale and returns a pointer to the Column object.
     * Use Column::unsigned() on the returned Column object to make the column unsigned.
     * \par MySQL statement
     * \code{.sql}
     * columnName DOUBLE(precision, scale)
     * \endcode
     */
    Column* doubleCol(const QString &columnName, uint precision, uint scale);
    /*!
     * \brief Creates/modifies a bit column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName BIT
     * \endcode
     */
    Column* bit(const QString &columnName);
    /*!
     * \brief Creates/modifies a date column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName DATE
     * \endcode
     */
    Column* date(const QString &columnName);
    /*!
     * \brief Creates/modifies a datetime column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName DATETIME
     * \endcode
     */
    Column* dateTime(const QString &columnName);
    /*!
     * \brief Creates/modifies a timestamp column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName TIMESTAMP
     * \endcode
     */
    Column* timestamp(const QString &columnName);
    /*!
     * \brief Creates/modifies a time column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName TIME
     * \endcode
     */
    Column* time(const QString &columnName);
    /*!
     * \brief Creates/modifies a year column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName YEAR
     * \endcode
     */
    Column* year(const QString &columnName);
    /*!
     * \brief Creates/modifies a binary column with given \a columnName and \a length and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName BINARY(\a length)
     * \endcode
     */
    Column* binary(const QString &columnName, uint length = 255);
    /*!
     * \brief Creates/modifies a varbinary column with given \a columnName and \a length and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName VARBINARY(length)
     * \endcode
     */
    Column* varBinary(const QString &columnName, uint length = 255);
    /*!
     * \brief Creates/modifies a tiny blob column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName TINYBLOB
     * \endcode
     */
    Column* tinyBlob(const QString &columnName);
    /*!
     * \brief Creates/modifies a blob column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName BLOB
     * \endcode
     */
    Column* blob(const QString &columnName);
    /*!
     * \brief Creates/modifies a medium blob column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName MEDIUMBLOB
     * \endcode
     */
    Column* mediumBlob(const QString &columnName);
    /*!
     * \brief Creates/modifies a long blob column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName LONGBLOB
     * \endcode
     */
    Column* longBlob(const QString &columnName);
    /*!
     * \brief Creates/modifies a char column with given \a columnName and \a length and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName CHAR(length)
     * \endcode
     */
    Column* charCol(const QString &columnName, uint length = 255);
    /*!
     * \brief Creates/modifies a varchar column with given \a columnName and \a length and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName VARCHAR(length)
     * \endcode
     */
    Column* varChar(const QString &columnName, uint length = 255);
    /*!
     * \brief Creates/modifies a tiny text column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName TINYTEXT
     * \endcode
     */
    Column* tinyText(const QString &columnName);
    /*!
     * \brief Creates/modifies a text column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName TEXT
     * \endcode
     */
    Column* text(const QString &columnName);
    /*!
     * \brief Creates/modifies a medium text column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName MEDIUMTEXT
     * \endcode
     */
    Column* mediumText(const QString &columnName);
    /*!
     * \brief Creates/modifies a long text column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName LONGTEXT
     * \endcode
     */
    Column* longText(const QString &columnName);
    /*!
     * \brief Creates/modifies an enum column with given \a columnName and \a enums and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName ENUM('enum1', 'enum2')
     * \endcode
     */
    Column* enumCol(const QString &columnName, const QStringList &enums);
    /*!
     * \brief Creates/modifies a set column with given \a columnName and \a setList and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName SET('entry1', 'entry2')
     * \endcode
     */
    Column* set(const QString &columnName, const QStringList &setList);
    /*!
     * \brief Creates/modifies a boolean column with given \a columnName and returns a pointer to the Column object.
     * \par MySQL statement
     * \code{.sql}
     * columnName TINYINT(1)
     * \endcode
     */
    Column* boolean(const QString &columnName);
    /*!
     * \brief Creates/modifies a primary key index over the columns \a cols named \a constraintSymbol and returns a pointer to the Column object.
     * The \a constraintSymbol is optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * [CONSTRAINT constraintSymbol] PRIMARY KEY (col1,col2)
     * \endcode
     */
    Column* primaryKey(const QStringList &cols, const QString &constraintSymbol = QString());
    /*!
     * \brief Creates/modifies a primary key index for \a col named \a constraintSymbol and returns a pointer to the Column object.
     * The \a constraintSymbol is optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * [CONSTRAINT constraintSymbol] PRIMARY KEY (col)
     * \endcode
     */
    Column* primaryKey(const QString &col, const QString &constraintSymbol = QString());
    /*!
     * \brief Creates/modifies a key index over the columns \a cols named \a indexName and returns a pointer to the Column object.
     * The \a indexName is optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * KEY [indexName] (col1,col2)
     * \endcode
     */
    Column* key(const QStringList &cols, const QString &indexName = QString());
    /*!
     * \brief Creates/modifies a key index for \a col named \a indexName and returns a pointer to the Column object.
     * The \a indexName is optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * KEY [indexName] (col)
     * \endcode
     */
    Column* key(const QString &col, const QString &indexName = QString());
    /*!
     * \brief Creates/modifies an index over the columns \a cols named \a indexName and returns a pointer to the Column object.
     * The \a indexName is optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * KEY [indexName] (col1,col2)
     * \endcode
     */
    Column* index(const QStringList &cols, const QString &indexName = QString());
    /*!
     * \brief Creates/modifies an index for \a col named \a indexName and returns a pointer to the Column object.
     * The \a indexName is optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * KEY [indexName] (col)
     * \endcode
     */
    Column* index(const QString &col, const QString &indexName = QString());
    /*!
     * \brief Creates/modifies a unique key index over the columns \a cols with \a constraintSymbol named \a indexName and returns a pointer to the Column object.
     * \a constraintSymbol and \a indexName are optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * [CONSTRAINT constraintSymbol] UNIQUE KEY [indexName] (col1,col2)
     * \endcode
     */
    Column* uniqueKey(const QStringList &cols, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    /*!
     * \brief Creates/modifies a unique key index for \a col with \a constraintSymbol named \a indexName and returns a pointer to the Column object.
     * \a constraintSymbol and \a indexName are optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * [CONSTRAINT constraintSymbol] UNIQUE KEY [indexName] (col)
     * \endcode
     */
    Column* uniqueKey(const QString &col, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    /*!
     * \brief Creates/modifies a unique index over the columns \a cols with \a constraintSymbol named \a indexName and returns a pointer to the Column object.
     * \a constraintSymbol and \a indexName are optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * [CONSTRAINT constraintSymbol] UNIQUE KEY [indexName] (col1,col2)
     * \endcode
     */
    Column* uniqueIndex(const QStringList &cols, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    /*!
     * \brief Creates/modifies a unique index for \a col with \a constraintSymbol named \a indexName and returns a pointer to the Column object.
     * \a constraintSymbol and \a indexName are optional but helps to identify the index in the future.
     * \par MySQL statement
     * \code{.sql}
     * [CONSTRAINT constraintSymbol] UNIQUE KEY [indexName] (col)
     * \endcode
     */
    Column* uniqueIndex(const QString &col, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    /*!
     * \brief Creates/modifies a foreign key constraint for \a foreignKeys that references the \a referenceCols at \a referenceTable and returns a pointer to the Column object.
     * \a constraintSymbol and \a indexName are optional but helps to identify the index in the future.
     * Use Column:onDelete() and/or Column::onUpdate() on the returned Column object to set reference behavior.
     * \par Example
     * \code{.cpp}
     * table->foreignKey(QStringList({"col1","col2"}), QStringLiteral("referenceTable"), QStringList({"refCol1","refCol2"}))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));
     * \endcode
     * \par MySQL statement
     * \code{.sql}
     * [CONSTRAINT constraintSymbol] FOREIGN KEY [indexName] (col1,col2) REFERENCES referenceTable (refCol1,refCol2) ON DELETE CASCADE ON UPDATE CASCADE
     * \endcode
     */
    Column* foreignKey(const QStringList &foreignKeys, const QString &referenceTable, const QStringList &referenceCols, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    /*!
     * \brief Creates/modifies a foreign key constraint for \a foreignKey that references the \a referenceCol at \a referenceTable and returns a pointer to the Column object.
     * \a constraintSymbol and \a indexName are optional but helps to identify the index in the future.
     * Use Column:onDelete() and/or Column::onUpdate() on the returned Column object to set reference behavior.
     * \par Example
     * \code{.cpp}
     * table->foreignKey(QStringLiteral("col"), QStringLiteral("referenceTable"), QStringLiteral("refCol"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));
     * \endcode
     * \par MySQL statement
     * \code{.sql}
     * [CONSTRAINT constraintSymbol] FOREIGN KEY [indexName] (col) REFERENCES referenceTable (refCol) ON DELETE CASCADE ON UPDATE CASCADE
     * \endcode
     */
    Column* foreignKey(const QString &foreignKeyCol, const QString &referenceTable, const QString &referenceCol, const QString &constraintSymbol = QString(), const QString &indexName = QString());

    /*!
     * \brief Drops the column identfied by \a columnName from the table.
     */
    void dropColumn(const QString &columnName);
};

}

#endif // FIRFUORIDA_TABLE_H
