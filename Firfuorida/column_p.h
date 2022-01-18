/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_COLUMN_P_H
#define FIRFUORIDA_COLUMN_P_H

#include "column.h"
#include "migrator.h"

namespace Firfuorida {

class ColumnPrivate {
public:
    enum Type : quint8 {
        TinyInt = 0,
        SmallInt,
        MediumInt,
        Int,
        BigInt,
        Decimal,
        Numeric,
        Float,
        Double,
        Bit,
        Date,
        DateTime,
        Timestamp,
        Time,
        Year,
        Binary,
        VarBinary,
        TinyBlob,
        Blob,
        MediumBlob,
        LongBlob,
        Char,
        VarChar,
        TinyText,
        Text,
        MediumText,
        LongText,
        Enum,
        Set,
        Boolean,
        Key,
        FulltextIndex,
        SpatialIndex,
        PrimaryKey,
        UniqueKey,
        ForeignKey
    };

    enum ColumnOperation : quint8 {
        CreateColumn,
        AddColumn,
        ModifyColumn,
        DropColumn
    };

    QString typeString() const;
    QString queryString() const;
    QString defValString() const;

    Migrator::DatabaseType dbType() const;
    QString dbTypeToStr() const;
    QVersionNumber dbVersion() const;
    Migrator::DatabaseFeatures dbFeatures() const;
    bool isDbFeatureAvailable(Migrator::DatabaseFeatures dbFeatures) const;

    QString charset;
    QString collation;
    QString after;
    QString indexName;
    QString referenceTable;
    QString onDelete;
    QString onUpdate;
    QString comment;
    QVariant defVal;
    QStringList constraintCols;
    QStringList referenceCols;
    QStringList enumSet;
    Column *q_ptr = nullptr;
    uint precision = 10;
    uint scale = 0;
    uint length = 255;
    uint displayWidth = 0;
    Type type = Int;
    ColumnOperation operation = CreateColumn;
    bool _unsigned = false;
    bool _nullable = false;
    bool _primaryKey = false;
    bool _autoIncrement = false;
    bool _useCurrent = false;
    bool _first = false;
    bool _unique = false;
    Q_DECLARE_PUBLIC(Column)
};

}

#endif // FIRFUORIDA_COLUMN_P_H
