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

class FIRFUORIDA_LIBRARY Table : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Table)
    friend class Migration;
    friend class MigrationPrivate;
    const QScopedPointer<TablePrivate> dptr;
    Q_DECLARE_PRIVATE_D(dptr, Table)
    explicit Table(QObject *parent);
public:
    ~Table() override;

    void setEngine(const QString &engine);
    void setCharset(const QString &charset);
    void setCollation(const QString &collation);

    Column* tinyIncrements(const QString &columnName = QStringLiteral("id"));
    Column* tinyInteger(const QString &columnName, uint displayWidth = 0);
    Column* smallIncrements(const QString &columnName = QStringLiteral("id"));
    Column* smallInteger(const QString &columnName, uint displayWidth = 0);
    Column* mediumIncrements(const QString &columnName = QStringLiteral("id"));
    Column* mediumInteger(const QString &columnName, uint displayWidth = 0);
    Column* increments(const QString &columnName = QStringLiteral("id"));
    Column* integer(const QString &columnName, uint displayWidth = 0);
    Column* bigIncrements(const QString &columnName = QStringLiteral("id"));
    Column* bigInteger(const QString &columnName, uint displayWidth = 0);
    Column* decimal(const QString &columnName, uint precision, uint scale);
    Column* numeric(const QString &columnName, uint precision, uint scale);
    Column* floatCol(const QString &columnName, uint precision, uint scale);
    Column* doubleCol(const QString &columnName, uint precision, uint scale);
    Column* bit(const QString &columnName);
    Column* date(const QString &columnName);
    Column* dateTime(const QString &columnName);
    Column* timestamp(const QString &columnName);
    Column* time(const QString &columnName);
    Column* year(const QString &columnName);
    Column* binary(const QString &columnName, uint length = 255);
    Column* varBinary(const QString &columnName, uint length = 255);
    Column* tinyBlob(const QString &columnName);
    Column* blob(const QString &columnName);
    Column* mediumBlob(const QString &columnName);
    Column* longBlob(const QString &columnName);
    Column* charCol(const QString &columnName, uint length = 255);
    Column* varChar(const QString &columnName, uint length = 255);
    Column* tinyText(const QString &columnName);
    Column* text(const QString &columnName);
    Column* mediumText(const QString &columnName);
    Column* longText(const QString &columnName);
    Column* enumCol(const QString &columnName, const QStringList &enums);
    Column* set(const QString &columnName, const QStringList &setList);
    Column* boolean(const QString &columnName);
    Column* primaryKey(const QStringList &cols, const QString &constraintSymbol = QString());
    Column* primaryKey(const QString &col, const QString &constraintSymbol = QString());
    Column* key(const QStringList &cols, const QString &indexName = QString());
    Column* key(const QString &col, const QString &indexName = QString());
    Column* index(const QStringList &cols, const QString &indexName = QString());
    Column* index(const QString &col, const QString &indexName = QString());
    Column* uniqueKey(const QStringList &cols, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    Column* uniqueKey(const QString &col, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    Column* uniqueIndex(const QStringList &cols, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    Column* uniqueIndex(const QString &col, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    Column* foreignKey(const QStringList &foreignKeys, const QString &referenceTable, const QStringList &referenceCols, const QString &constraintSymbol = QString(), const QString &indexName = QString());
    Column* foreignKey(const QString &foreignKeyCol, const QString &referenceTable, const QString &referenceCol, const QString &constraintSymbol = QString(), const QString &indexName = QString());
};

}

#endif // FIRFUORIDA_TABLE_H
