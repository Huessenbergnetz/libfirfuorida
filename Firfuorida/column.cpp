/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "column_p.h"
#include <QStringList>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QBitArray>
#include "logging.h"
#include "table.h"
#include "migration.h"

using namespace Firfuorida;

QString ColumnPrivate::typeString() const
{
    QString str;
    if (dbType() == Migrator::MariaDB || dbType() == Migrator::MySQL) {
        switch(type) {
        case TinyInt:
            str = QStringLiteral("TINYINT");
            break;
        case SmallInt:
            str = QStringLiteral("SMALLINT");
            break;
        case MediumInt:
            str = QStringLiteral("MEDIUMINT");
            break;
        case Int:
            str = QStringLiteral("INT");
            break;
        case BigInt:
            str = QStringLiteral("BIGINT");
            break;
        case Decimal:
            str = QStringLiteral("DECIMAL") + QLatin1Char('(') + QString::number(precision) + QLatin1Char(',') + QString::number(scale) + QLatin1Char(')');
            break;
        case Numeric:
            str = QStringLiteral("NUMERIC") + QLatin1Char('(') + QString::number(precision) + QLatin1Char(',') + QString::number(scale) + QLatin1Char(')');
            break;
        case Float:
            str = QStringLiteral("FLOAT") + QLatin1Char('(') + QString::number(precision) + QLatin1Char(',') + QString::number(scale) + QLatin1Char(')');
            break;
        case Double:
            str = QStringLiteral("DOUBLE") + QLatin1Char('(') + QString::number(precision) + QLatin1Char(',') + QString::number(scale) + QLatin1Char(')');
            break;
        case Bit:
            str = QStringLiteral("BIT");
            break;
        case Date:
            str = QStringLiteral("DATE");
            break;
        case DateTime:
            str = QStringLiteral("DATETIME");
            break;
        case Timestamp:
            str = QStringLiteral("TIMESTAMP");
            break;
        case Time:
            str = QStringLiteral("TIME");
            break;
        case Year:
            str = QStringLiteral("YEAR");
            break;
        case Binary:
            str = QStringLiteral("BINARY") + QLatin1Char('(') + QString::number(length) + QLatin1Char(')');
            break;
        case VarBinary:
            str = QStringLiteral("VARBINARY") + QLatin1Char('(') + QString::number(length) + QLatin1Char(')');
            break;
        case TinyBlob:
            str = QStringLiteral("TINYBLOB");
            break;
        case Blob:
            str = QStringLiteral("BLOB");
            break;
        case MediumBlob:
            str = QStringLiteral("MEDIUMBLOB");
            break;
        case LongBlob:
            str = QStringLiteral("LONGBLOB");
            break;
        case Char:
            str = QStringLiteral("CHAR") + QLatin1Char('(') + QString::number(length) + QLatin1Char(')');
            break;
        case VarChar:
            str = QStringLiteral("VARCHAR") + QLatin1Char('(') + QString::number(length) + QLatin1Char(')');
            break;
        case TinyText:
            str = QStringLiteral("TINYTEXT");
            break;
        case Text:
            str = QStringLiteral("TEXT");
            break;
        case MediumText:
            str = QStringLiteral("MEDIUMTEXT");
            break;
        case LongText:
            str = QStringLiteral("LONGTEXT");
            break;
        case Json:
            str = QStringLiteral("JSON");
            break;
        case Enum:
            str = QStringLiteral("ENUM");
            break;
        case Set:
            str = QStringLiteral("SET");
            break;
        case Boolean:
            str = QStringLiteral("TINYINT(1)");
            break;
        case Key:
            str = QStringLiteral("KEY");
            break;
        case FulltextIndex:
            str = QStringLiteral("FULLTEXT INDEX");
            break;
        case SpatialIndex:
            str = QStringLiteral("SPATIAL INDEX");
            break;
        case PrimaryKey:
            str = QStringLiteral("PRIMARY KEY");
            break;
        case UniqueKey:
            str = QStringLiteral("UNIQUE KEY");
            break;
        case ForeignKey:
            str = QStringLiteral("FOREIGN KEY");
            break;
        case Invalid:
            return str;
        }

        if (type == Enum || type == Set) {
            QStringList setList;
            setList.reserve(enumSet.size());
            const auto lst = enumSet;
            for (const QString &part : lst) {
                const QString _part = QLatin1Char('\'') + part + QLatin1Char('\'');
                setList << _part;
            }
            str += QLatin1Char('(') + setList.join(QStringLiteral(", ")) + QLatin1Char(')');
        }

        if (displayWidth > 0 && type < Decimal) {
            str += QLatin1Char('(') + QString::number(displayWidth) + QLatin1Char(')');
        }
    } else if(dbType() == Migrator::SQLite) {
        switch(type) {
        case TinyInt:
        case SmallInt:
        case MediumInt:
        case Int:
        case BigInt:
            str = QStringLiteral("INTEGER");
            break;
        case Char:
        case VarChar:
        case TinyText:
        case Text:
        case MediumText:
        case LongText:
        case Json:
        case Binary:
        case VarBinary:
            str = QStringLiteral("TEXT");
            break;
        case TinyBlob:
        case Blob:
        case MediumBlob:
        case LongBlob:
            str = QStringLiteral("BLOB");
            break;
        case Float:
        case Double:
            str = QStringLiteral("REAL");
            break;
        case Bit:
        case Decimal:
        case Numeric:
        case Date:
        case DateTime:
        case Timestamp:
        case Time:
        case Year:
        case Boolean:
            str = QStringLiteral("NUMERIC");
            break;
        case Enum:
        case Set:
            qCWarning(FIR_CORE, "%s", "ENUM and SET data types are not supported on SQLite databases.");
            break;
        case Key:
            str = QStringLiteral("KEY");
            break;
        case PrimaryKey:
            str = QStringLiteral("PRIMARY KEY");
            break;
        case UniqueKey:
            str = QStringLiteral("UNIQUE");
            break;
        case ForeignKey:
            str = QStringLiteral("FOREIGN KEY");
            break;
        case FulltextIndex:
        case SpatialIndex:
        case Invalid:
            return str;
        }
    }

    return str;
}

QString ColumnPrivate::defValString() const
{
    Q_Q(const Column);

    QString str;

    if (dbType() == Migrator::MySQL || dbType() == Migrator::MariaDB) {
        const QString defValStr = defVal.toString();
        if (defValStr.startsWith(QLatin1Char('(')) && defValStr.endsWith(QLatin1Char(')'))) {
            str = defValStr;
            return str;
        }
        if (defValStr.compare(QLatin1String("NULL"), Qt::CaseInsensitive) == 0) {
            if (_nullable) {
                str = defValStr;
            } else {
                qCWarning(FIR_CORE) << "Invalid default value NULL for column" << q->objectName() << "of type" << typeString() << ": not nullable";
            }
            return str;
        }

        if (type < Bit) { // numeric columns
            bool isDouble = false;
            bool isInteger = false;
            defVal.toDouble(&isDouble);
            defVal.toLongLong(&isInteger);
            if ((isDouble || isInteger) && defVal.canConvert<QString>()) {
                str = defVal.toString();
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type == Bit) {
            if (defVal.canConvert<QBitArray>()) {
                const QBitArray ba = defVal.toBitArray();
                if (!ba.isEmpty()) {
                    int length = ba.size();
                    if (length > 63) {
                        length = 63;
                    }
                    str.reserve(length + 4);
                    str[0] = QLatin1Char('\'');
                    str[1] = QLatin1Char('0');
                    str[2] = QLatin1Char('b');
                    for (int i = 0; i < length; ++i) {
                        str[i+3] = ba.at(i) ? QLatin1Char('1') : QLatin1Char('0');
                    }
                    str[length + 3] = QLatin1Char('\'');
                }
            } else if (defVal.canConvert<QString>()) {
                str = defVal.toString();
                if (!str.startsWith(QLatin1String("b'"))) {
                    str.prepend(QLatin1String("b'"));
                }
                if (!str.endsWith(QLatin1Char('\''))) {
                    str.append(QLatin1Char('\''));
                }
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type == Date) {
            if (defVal.canConvert<QDate>()) {
                str = QLatin1Char('\'') + defVal.toDate().toString(Qt::ISODate) + QLatin1Char('\'');
            } else if (defVal.canConvert<QString>()) {
                str = QLatin1Char('\'') + defVal.toString() + QLatin1Char('\'');
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type == DateTime || type == Timestamp) {
            if (defVal.type() == QMetaType::QDateTime) {
                str = QLatin1Char('\'') + defVal.toDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz")) + QLatin1Char('\'');
            } else if (defVal.canConvert<QString>()) {
                if (defValStr.compare(QLatin1String("CURRENT_TIMESTAMP"), Qt::CaseInsensitive) == 0) {
                    str = defValStr;
                } else {
                    str = QLatin1Char('\'') + defVal.toString() + QLatin1Char('\'');
                }
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type == Time) {
            if (defVal.canConvert<QTime>()) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
                str = QLatin1Char('\'') + defVal.toTime().toString(Qt::ISODateWithMs) + QLatin1Char('\'');
#else
                str = QLatin1Char('\'') + defVal.toTime().toString(QStringLiteral("HH:MM:ss.zzz")) + QLatin1Char('\'');
#endif
            } else if (defVal.canConvert<QString>()) {
                str = QLatin1Char('\'') + defVal.toString() + QLatin1Char('\'');
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type == Year) {
            if (defVal.canConvert<QString>()) {
                str = QLatin1Char('\'') + defVal.toString() + QLatin1Char('\'');
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type > Year && type < TinyBlob) { // binay type columns
            if (defVal.canConvert<QString>()) {
                str = QLatin1Char('\'') + defVal.toString() + QLatin1Char('\'');
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type >= TinyBlob && type <= LongBlob) { // blob type columns
            if (defVal.canConvert<QByteArray>()) {
                str = QLatin1String("('0x") + QString::fromLatin1(defVal.toByteArray().toHex()) + QLatin1String("')");
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type >= Char && type <= Json) { // char and text type columns
            if (defVal.canConvert<QString>()) {
                QString _str = defVal.toString();
                _str.replace(QLatin1Char('\''), QLatin1String("\\'"));
                str = QLatin1String("('") + _str + QLatin1String("')");
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type == Boolean) {
            if (defVal.canConvert<bool>()) {
                str = defVal.toBool() ? QLatin1Char('1') : QLatin1Char('0');
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        }

    } else if (dbType() == Migrator::SQLite) {

        QString defValStr = defVal.toString();
        if (!defValStr.isEmpty()) {
            if (defValStr.compare(QLatin1String("CURRENT_TIME"), Qt::CaseInsensitive) == 0 || defValStr.compare(QLatin1String("CURRENT_DATE"), Qt::CaseInsensitive) == 0 || defValStr.compare(QLatin1String("CURRENT_TIMESTAMP"), Qt::CaseInsensitive) == 0) {
                str = defValStr;
                return str;
            }
            if (defValStr.compare(QLatin1String("NULL"), Qt::CaseInsensitive) == 0) {
                if (_nullable) {
                    str = defValStr;
                } else {
                    qCWarning(FIR_CORE) << "Invalid default value NULL for column" << q->objectName() << "of type" << typeString() << ": not nullable";
                }
                return str;
            }
        }

        if (defValStr.startsWith(QLatin1Char('(')) && defValStr.endsWith(QLatin1Char(')'))) {
            str = defValStr;
            return str;
        }

        if (type < Bit) { // numeric values
            bool isInteger = false;
            bool isDouble = false;
            defVal.toLongLong(&isInteger);
            defVal.toDouble(&isDouble);
            if (isInteger || isDouble) {
                str = defValStr;
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if (type >= TinyBlob && type <= LongBlob) {
            if (defValStr.startsWith(QLatin1String("x'")) && defValStr.endsWith(QLatin1Char('\''))) {
                str = defValStr;
            } else if (defVal.canConvert<QByteArray>()) {
                const auto ba = defVal.toByteArray();
                str = QLatin1String("x'") + QString::fromLatin1(defVal.toByteArray().toHex()) + QLatin1Char('\'');
            } else {
                qCWarning(FIR_CORE) << "Invalid default value for column" << q->objectName() << "of type" << typeString() << ":" << defVal;
            }
        } else if ((type == Binary || type == VarBinary || (type >= Char && type <= LongText)) && defVal.canConvert<QString>()) {
            QString _str = defVal.toString();
            _str.replace(QLatin1Char('\''), QLatin1String("\\'"));
            str = QLatin1Char('\'') + _str + QLatin1Char('\'');
        }
    }

    return str;
}

QString ColumnPrivate::schemaAndColName() const
{
    Q_Q(const Column);
    return q->parent()->objectName() + QLatin1Char('.') + q->objectName();
}

QString ColumnPrivate::queryString() const
{
    QString qs;

    QStringList parts;
    Q_Q(const Column);

    if ((operation == CreateColumn || operation == AddColumn) && type < Key) {
        if (type == Invalid) {
            return qs;
        }

        if (operation == AddColumn) {
            parts << QStringLiteral("ADD") << QStringLiteral("COLUMN");
        }

        parts << q->objectName();
        parts << typeString();
        if (type < Bit && _unsigned && dbType() != Migrator::SQLite) {
            parts << QStringLiteral("UNSIGNED");
        }
        if (type > LongBlob && type < Enum) {
            if (!charset.isEmpty()) {
                const QString cs = QStringLiteral("CHARACTER SET ") + charset;
                parts << cs;
            }
            if (!collation.isEmpty()) {
                const QString cs = QStringLiteral("COLLATE ") + collation;
                parts << cs;
            }
        }
        if (_primaryKey || _autoIncrement) {
            parts << QStringLiteral("PRIMARY KEY");
        }
        if (_autoIncrement) {
            if (dbType() == Migrator::SQLite) {
                parts << QStringLiteral("AUTOINCREMENT");
            } else {
                parts << QStringLiteral("AUTO_INCREMENT");
            }
        }
        if (_unique) {
            if (dbType() == Migrator::SQLite) {
                parts << QStringLiteral("UNIQUE");
            } else {
                parts << QStringLiteral("UNIQUE KEY");
            }
        }
        if (!_nullable || _primaryKey) {
            parts << QStringLiteral("NOT NULL");
        }
        if (!defVal.isNull()) {
            const auto _def = defValString();
            if (!_def.isEmpty()) {
                parts << QStringLiteral("DEFAULT");
                parts << _def;
            }
        }
        if (!comment.isEmpty()) {
            const QString _comment = QLatin1Char('\'') + comment + QLatin1Char('\'');
            parts << QStringLiteral("COMMENT");
            parts << _comment;
        }
    }

    if (operation == CreateColumn && type >= Key && type <= ForeignKey) {
        if (type == Invalid) {
            return qs;
        }

        if (!q->objectName().isEmpty()) {
            parts << QStringLiteral("CONSTRAINT") << q->objectName();
        }
        parts << typeString();
        if (!indexName.isEmpty()) {
            parts << indexName;
        }
        const QString _keyCols = QLatin1Char('(') + constraintCols.join(QLatin1Char(',')) + QLatin1Char(')');
        parts << _keyCols;

        if (type == ForeignKey) {
            parts << QStringLiteral("REFERENCES") << referenceTable;
            const QString _refCols = QLatin1Char('(') + referenceCols.join(QLatin1Char(',')) + QLatin1Char(')');
            parts << _refCols;

            if (!onDelete.isEmpty()) {
                parts << QStringLiteral("ON DELETE") << onDelete;
            }

            if (!onUpdate.isEmpty()) {
                parts << QStringLiteral("ON UPDATE") << onUpdate;
            }
        }

        if (!comment.isEmpty()) {
            const QString _comment = QLatin1Char('\'') + comment + QLatin1Char('\'');
            parts << QStringLiteral("COMMENT");
            parts << _comment;
        }
    }

    if (operation == DropColumn && type < Key) {
        parts << QStringLiteral("DROP") << QStringLiteral("COLUMN") << q->objectName();
    }

    qs = parts.join(QChar(QChar::Space));

#ifndef QT_NO_DEBUG_OUTPUT
    switch (operation) {
    case CreateColumn:
        qCDebug(FIR_CORE, "Creating column: %s", qUtf8Printable(qs));
        break;
    case AddColumn:
        qCDebug(FIR_CORE, "Adding column: %s", qUtf8Printable(qs));
        break;
    case ModifyColumn:
        qCDebug(FIR_CORE, "Modifying column: %s", qUtf8Printable(qs));
        break;
    case DropColumn:
        qCDebug(FIR_CORE, "Dropping column: %s", qUtf8Printable(qs));
        break;
    }
#endif

    return qs;
}

Migrator::DatabaseType ColumnPrivate::dbType() const
{
    Q_Q(const Column);
    auto table = qobject_cast<Table*>(q->parent());
    auto migration = qobject_cast<Migration*>(table->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->dbType();
}

QString ColumnPrivate::dbTypeToStr() const
{
    Q_Q(const Column);
    auto table = qobject_cast<Table*>(q->parent());
    auto migration = qobject_cast<Migration*>(table->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->dbTypeToStr();
}

QVersionNumber ColumnPrivate::dbVersion() const
{
    Q_Q(const Column);
    auto table = qobject_cast<Table*>(q->parent());
    auto migration = qobject_cast<Migration*>(table->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->dbVersion();
}

Migrator::DatabaseFeatures ColumnPrivate::dbFeatures() const
{
    Q_Q(const Column);
    auto table = qobject_cast<Table*>(q->parent());
    auto migration = qobject_cast<Migration*>(table->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->dbFeatures();
}

bool ColumnPrivate::isDbFeatureAvailable(Migrator::DatabaseFeatures dbFeatures) const
{
    Q_Q(const Column);
    auto table = qobject_cast<Table*>(q->parent());
    auto migration = qobject_cast<Migration*>(table->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->isDbFeatureAvailable(dbFeatures);
}

Column::Column(Table *parent) : QObject(parent), dptr(new ColumnPrivate)
{
    Q_D(Column);
    d->q_ptr = this;
}

Column::~Column()
{

}

Column* Column::autoIncrement(bool autoIncrement)
{
    Q_D(Column);
    if (d->type < ColumnPrivate::Decimal || (d->type > ColumnPrivate::Numeric && d->type < ColumnPrivate::Bit)) {
        d->_autoIncrement = autoIncrement;
    } else {
        qCWarning(FIR_CORE, "autoIncrement() / AUTO_INCREMENT attribute is only usable on integer and floating-point data type columns. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
    }
    return this;
}

Column* Column::unSigned(bool unSigned)
{
    Q_D(Column);
    if (d->isDbFeatureAvailable(Migrator::UnsignedInteger)) {
        if (d->type < ColumnPrivate::Bit) {
            d->_unsigned = unSigned;
        } else {
            qCWarning(FIR_CORE, "unSigned() / UNSIGNED attribute is only usable on numeric data type columns. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
        }
    } else {
        d->_unsigned = false;
        qCWarning(FIR_CORE, "Can not set \"%s\" as UNSIGNED %s. %s %s does not support UNSIGNED values.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()), qUtf8Printable(d->dbTypeToStr()), qUtf8Printable(d->dbVersion().toString()));
    }
    return this;
}

Column* Column::charset(const QString &charset)
{
    Q_D(Column);
    if (d->isDbFeatureAvailable(Migrator::CharsetOnColumn)) {
        if (d->type >= ColumnPrivate::Char && d->type <= ColumnPrivate::Set) {
            d->charset = charset;
        } else {
            qCWarning(FIR_CORE, "charset() / CHARACTER SET attribute is only usable on character data type columns. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
        }
    } else {
        qCWarning(FIR_CORE, "Can not set CHARACTER SET on column %s. %s %s only supports a per database charset set by PRAGMA encoding.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->dbTypeToStr()), qUtf8Printable(d->dbVersion().toString()));
        d->charset.clear();
    }



    return this;
}

Column* Column::collation(const QString &collation)
{
    Q_D(Column);
    if (d->type >= ColumnPrivate::Char && d->type <= ColumnPrivate::Set) {
        d->collation = collation;
    } else {
        qCWarning(FIR_CORE, "collation() / COLLATE attribute is only usable on character data type columns. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
    }
    return this;
}

Column* Column::defaultValue(const QVariant &defVal)
{
    Q_D(Column);
    if (d->type < ColumnPrivate::Key) {
        if ((d->type > ColumnPrivate::VarBinary && d->type < ColumnPrivate::Char) && !d->isDbFeatureAvailable(Migrator::DefValOnBlob)) {
            qCWarning(FIR_CORE, "%s %s does not support DEFAULT values on BLOB type columns. \"%s\" is of type %s.", qUtf8Printable(d->dbTypeToStr()), qUtf8Printable(d->dbVersion().toString()), qUtf8Printable(d->schemaAndColName()),  qUtf8Printable(d->typeString()));
            return this;
        }
        if ((d->type > ColumnPrivate::VarChar && d->type < ColumnPrivate::Enum) && !d->isDbFeatureAvailable(Migrator::DefValOnText)) {
            qCWarning(FIR_CORE, "%s %s does not support DEFAULT values on TEXT type columns. \"%s\" is of type %s.", qUtf8Printable(d->dbTypeToStr()), qUtf8Printable(d->dbVersion().toString()), qUtf8Printable(d->schemaAndColName()),  qUtf8Printable(d->typeString()));
            return this;
        }
        d->defVal = defVal;
    } else {
        qCWarning(FIR_CORE, "defaultValue() / DEFAULT attribute is only usable on data type columns. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
    }
    return this;
}

Column *Column::nullable(bool isNullable)
{
    Q_D(Column);
    if (d->type < ColumnPrivate::Key) {
        d->_nullable = isNullable;
    } else {
        qCWarning(FIR_CORE, "nullable() / NOT NULL attribute is only usable on data type columns. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
    }
    return this;
}

Column *Column::useCurrent(bool useCurrent)
{
    Q_D(Column);
    d->_useCurrent = useCurrent;
    return this;
}

Column *Column::primary(bool primary)
{
    Q_D(Column);
    if (d->type < ColumnPrivate::Key) {
        d->_primaryKey = primary;
    } else {
        qCWarning(FIR_CORE, "primary() / PRIMARY KEY attribute is only usable on data type columns. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
    }
    return this;
}

Column *Column::unique(bool unique)
{
    Q_D(Column);
    if (d->type < ColumnPrivate::Key) {
        d->_unique = unique;
    } else {
        qCWarning(FIR_CORE, "unique() / UNIQUE attribute is only usable on data type columns. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
    }
    return this;
}

Column *Column::onDelete(const QString &referenceOption)
{
    Q_D(Column);
    if (d->type == ColumnPrivate::ForeignKey) {
        d->onDelete = referenceOption;
    } else {
        qCWarning(FIR_CORE, "onDelete() / ON DELETE reference option is only usable on FOREIGN KEY constraints. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
    }
    return this;
}

Column *Column::onUpdate(const QString &referenceOption)
{
    Q_D(Column);
    if (d->type == ColumnPrivate::ForeignKey) {
        d->onUpdate = referenceOption;
    } else {
        qCWarning(FIR_CORE, "onUpdate() / ON UPDATE reference option is only usable on FOREIGN KEY constraints. \"%s\" is of type %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->typeString()));
    }
    return this;
}

Column *Column::comment(const QString &comment)
{
    Q_D(Column);
    if (d->isDbFeatureAvailable(Migrator::CommentsOnColumns)) {
        QString _comment = comment;
        _comment.replace(QLatin1Char('\''), QLatin1String("\\'"));
        if (_comment.size() > 1024) {
            qCWarning(FIR_CORE, "%s: comment() / COMMENT can not exceed 1024 characters. Your comment is %i characters long. It will be truncated.", qUtf8Printable(d->schemaAndColName()), _comment.size());
            _comment = _comment.left(1024);
        }
        d->comment = _comment;
    } else {
        qCWarning(FIR_CORE, "%s: Setting comments on columns is not supported by %s %s.", qUtf8Printable(d->schemaAndColName()), qUtf8Printable(d->dbTypeToStr()), qUtf8Printable(d->dbVersion().toString()));
        d->comment.clear();
    }
    return this;
}

void Column::after(const QString &otherColumn)
{
    Q_D(Column);
    d->after = otherColumn;
}

void Column::first()
{
    Q_D(Column);
    d->_first = true;
}

void Column::change()
{
    Q_D(Column);
    d->operation = ColumnPrivate::ModifyColumn;
}

#include "moc_column.cpp"
