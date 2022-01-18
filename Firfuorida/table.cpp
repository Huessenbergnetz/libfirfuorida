/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "table_p.h"
#include "column_p.h"
#include <QStringList>
#include "logging.h"
#include "migration.h"

using namespace Firfuorida;

QString TablePrivate::queryString() const
{
    QString qs;

    Q_Q(const Table);

    if (operation == CreateTable || operation == CreateTableIfNotExists) {
        qs = QStringLiteral("CREATE TABLE ");

        if (operation == CreateTableIfNotExists) {
            qs += QStringLiteral("IF NOT EXISTS ");
        }

        qs += q->objectName() + QLatin1Char('(');

        const QList<Column *> cols = q->findChildren<Column *>(QString(), Qt::FindDirectChildrenOnly);
        QStringList colParts;
        for (Column *col : cols) {
            const QString qs = col->d_func()->queryString();
            if (!qs.isEmpty()) {
                colParts << qs;
            }
        }
        qs += colParts.join(QStringLiteral(", "));

        qs += QLatin1Char(')');
        if (!engine.isEmpty()) {
            qs += QStringLiteral(" ENGINE = ") + engine;
        }
        if (!charset.isEmpty()) {
            qs += QStringLiteral(" DEFAULT CHARSET = ") + charset;
        }
        if (!collation.isEmpty()) {
            qs += QStringLiteral(" COLLATE = ") + collation;
        }
        if (!comment.isEmpty()) {
            qs += QStringLiteral(" COMMENT = \"") + comment + QStringLiteral("\"");
        }
    } else if (operation == DropTable) {
        qs = QStringLiteral("DROP TABLE ") + q->objectName();
    } else if (operation == DropTableIfExists) {
        qs = QStringLiteral("DROP TABLE IF EXISTS ") + q->objectName();
    } else if (operation == Raw) {
        qs = raw;
    }

    return qs;
}

Migrator::DatabaseType TablePrivate::dbType() const
{
    Q_Q(const Table);
    auto migration = qobject_cast<Migration*>(q->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->dbType();
}

QString TablePrivate::dbTypeToStr() const
{
    Q_Q(const Table);
    auto migration = qobject_cast<Migration*>(q->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->dbTypeToStr();
}

QVersionNumber TablePrivate::dbVersion() const
{
    Q_Q(const Table);
    auto migration = qobject_cast<Migration*>(q->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->dbVersion();
}

Migrator::DatabaseFeatures TablePrivate::dbFeatures() const
{
    Q_Q(const Table);
    auto migration = qobject_cast<Migration*>(q->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->dbFeatures();
}

bool TablePrivate::isDbFeatureAvailable(Migrator::DatabaseFeatures dbFeatures) const
{
    Q_Q(const Table);
    auto migration = qobject_cast<Migration*>(q->parent());
    auto migrator = qobject_cast<Migrator*>(migration->parent());
    return migrator->isDbFeatureAvailable(dbFeatures);
}

Table::Table(Firfuorida::Migration *parent) : QObject(parent), dptr(new TablePrivate)
{
    Q_D(Table);
    d->q_ptr = this;
}

Table::~Table()
{

}

void Table::setEngine(const QString &engine)
{
    Q_D(Table);
    d->engine = engine;
}

void Table::setCharset(const QString &charset)
{
    Q_D(Table);
    d->charset = charset;
}

void Table::setCollation(const QString &collation)
{
    Q_D(Table);
    d->collation = collation;
}

void Table::setComment(const QString &comment)
{
    Q_D(Table);
    QString _comment = comment;
    _comment.replace(QLatin1Char('\''), QLatin1String("\\'"));
    if (_comment.size() > 2048) {
        qCWarning(FIR_CORE, "setComment() / COMMENT can not exceed 2048 characters. Your comment is %i characters long. It will be truncated.", _comment.size());
        _comment = _comment.left(2048);
    }
    d->comment = _comment;
}

Column* Table::tinyIncrements(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "tiny increments column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::TinyInt;
    c->d_func()->_autoIncrement = true;
    c->d_func()->_unsigned = true;
    return c;
}

Column* Table::tinyInteger(const QString &columnName, uint displayWidth)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "tiny integer column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::TinyInt;
    c->d_func()->displayWidth = displayWidth;
    return c;
}

Column* Table::smallIncrements(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "small increments column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::SmallInt;
    c->d_func()->_autoIncrement = true;
    c->d_func()->_unsigned = true;
    return c;
}

Column* Table::smallInteger(const QString &columnName, uint displayWidth)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "small integer column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::SmallInt;
    c->d_func()->displayWidth = displayWidth;
    return c;
}

Column* Table::mediumIncrements(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "medium increments column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::MediumInt;
    c->d_func()->_autoIncrement = true;
    c->d_func()->_unsigned = true;
    return c;
}

Column* Table::mediumInteger(const QString &columnName, uint displayWidth)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "medium integer column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::MediumInt;
    c->d_func()->displayWidth = displayWidth;
    return c;
}

Column* Table::increments(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "increments column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Int;
    c->d_func()->_autoIncrement = true;
    c->d_func()->_unsigned = true;
    return c;
}

Column* Table::integer(const QString &columnName, uint displayWidth)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "integer column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Int;
    c->d_func()->displayWidth = displayWidth;
    return c;
}

Column* Table::bigIncrements(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "big increments column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::BigInt;
    c->d_func()->_autoIncrement = true;
    c->d_func()->_unsigned = true;
    return c;
}

Column* Table::bigInteger(const QString &columnName, uint displayWidth)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "big integer column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::BigInt;
    c->d_func()->displayWidth = displayWidth;
    return c;
}

Column* Table::decimal(const QString &columnName, uint precision, uint scale)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "decimal column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Decimal;
    c->d_func()->precision = precision;
    c->d_func()->scale = scale;
    return c;
}

Column* Table::numeric(const QString &columnName, uint precision, uint scale)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "numeric column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Numeric;
    c->d_func()->precision = precision;
    c->d_func()->scale = scale;
    return c;
}

Column* Table::floatCol(const QString &columnName, uint precision, uint scale)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "float column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Float;
    c->d_func()->precision = precision;
    c->d_func()->scale = scale;
    return c;
}

Column* Table::doubleCol(const QString &columnName, uint precision, uint scale)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "double column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Double;
    c->d_func()->precision = precision;
    c->d_func()->scale = scale;
    return c;
}

Column* Table::bit(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "bit column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Bit;
    return c;
}

Column* Table::date(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "date column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Date;
    return c;
}

Column* Table::dateTime(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "datetime column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::DateTime;
    return c;
}

Column* Table::timestamp(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "timestamp column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Timestamp;
    return c;
}

Column* Table::time(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "time column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Time;
    return c;
}

Column* Table::year(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "year column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Year;
    return c;
}

Column *Table::binary(const QString &columnName, uint length)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "binary column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Binary;
    c->d_func()->length = length;
    return c;
}

Column *Table::varBinary(const QString &columnName, uint length)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "varbinary column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::VarBinary;
    c->d_func()->length = length;
    return c;
}

Column* Table::tinyBlob(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "tinyblob column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::TinyBlob;
    return c;
}

Column* Table::blob(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "blob column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Blob;
    return c;
}

Column* Table::mediumBlob(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "medium blob column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::MediumBlob;
    return c;
}

Column* Table::longBlob(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "long blob column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::LongBlob;
    return c;
}

Column *Table::charCol(const QString &columnName, uint length)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "char column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Char;
    c->d_func()->length = length;
    return c;
}

Column *Table::varChar(const QString &columnName, uint length)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "varchar column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::VarChar;
    c->d_func()->length = length;
    return c;
}

Column* Table::tinyText(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "tinytext column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::TinyText;
    return c;
}

Column* Table::text(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "text column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Text;
    return c;
}

Column* Table::mediumText(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "medium text column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::MediumText;
    return c;
}

Column* Table::longText(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "long text column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::LongText;
    return c;
}

Column* Table::enumCol(const QString &columnName, const QStringList &enums)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "enum column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    if (d->isDbFeatureAvailable(Migrator::EnumType)) {
        c->d_func()->type = ColumnPrivate::Enum;
        c->d_func()->enumSet = enums;
    } else {
        qCWarning(FIR_CORE, "%s %s does not support the ENUM data type. Omitting column \"%s\".", qUtf8Printable(d->dbTypeToStr()), qUtf8Printable(d->dbVersion().toString()), qUtf8Printable(objectName()));
    }
    return c;
}

Column* Table::set(const QString &columnName, const QStringList &setList)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "set column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    if (d->isDbFeatureAvailable(Migrator::EnumType)) {
        c->d_func()->type = ColumnPrivate::Set;
        c->d_func()->enumSet = setList;
    } else {
        qCWarning(FIR_CORE, "%s %s does not support the SET data type. Omitting column \"%s\".", qUtf8Printable(d->dbTypeToStr()), qUtf8Printable(d->dbVersion().toString()), qUtf8Printable(objectName()));
    }
    return c;
}

Column* Table::boolean(const QString &columnName)
{
    auto c = new Column(this);
    Q_ASSERT_X(!columnName.trimmed().isEmpty(), "boolean column", "column name can not be empty");
    c->setObjectName(columnName.trimmed());
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Boolean;
    return c;
}

Column* Table::primaryKey(const QStringList &cols, const QString &constraintSymbol)
{
    auto c = new Column(this);
    c->setObjectName(constraintSymbol);
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::PrimaryKey;
    c->d_func()->constraintCols = cols;
    return c;
}

Column* Table::primaryKey(const QString &col, const QString &constraintSymbol)
{
    return primaryKey(QStringList(col), constraintSymbol);
}

Column* Table::key(const QStringList &cols, const QString &indexName)
{
    auto c = new Column(this);
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::Key;
    c->d_func()->constraintCols = cols;
    c->d_func()->indexName = indexName.trimmed();
    return c;
}

Column *Table::key(const QString &col, const QString &indexName)
{
    return key(QStringList(col), indexName);
}

Column* Table::index(const QStringList &cols, const QString &indexName)
{
    return key(cols, indexName);
}

Column* Table::index(const QString &col, const QString &indexName)
{
    return key(QStringList(col), indexName);
}

Column* Table::uniqueKey(const QStringList &cols, const QString &constraintSymbol, const QString &indexName)
{
    auto c = new Column(this);
    c->setObjectName(constraintSymbol);
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::UniqueKey;
    c->d_func()->constraintCols = cols;
    c->d_func()->indexName = indexName;
    return c;
}

Column* Table::uniqueKey(const QString &col, const QString &constraintSymbol, const QString &indexName)
{
    return uniqueKey(QStringList(col), constraintSymbol, indexName);
}

Column* Table::uniqueIndex(const QStringList &cols, const QString &constraintSymbol, const QString &indexName)
{
    return uniqueKey(cols, constraintSymbol, indexName);
}

Column* Table::uniqueIndex(const QString &col, const QString &constraintSymbol, const QString &indexName)
{
    return uniqueKey(QStringList(col), constraintSymbol, indexName);
}

Column* Table::foreignKey(const QStringList &foreignKeys, const QString &referenceTable, const QStringList &referenceCols, const QString &constraintSymbol, const QString &indexName)
{
    auto c = new Column(this);
    c->setObjectName(constraintSymbol);
    Q_D(Table);
    c->d_func()->operation = (d->operation == TablePrivate::CreateTable || d->operation == TablePrivate::CreateTableIfNotExists) ? ColumnPrivate::CreateColumn : ColumnPrivate::AddColumn;
    c->d_func()->type = ColumnPrivate::ForeignKey;
    c->d_func()->constraintCols = foreignKeys;
    c->d_func()->referenceTable = referenceTable.trimmed();
    c->d_func()->referenceCols = referenceCols;
    c->d_func()->indexName = indexName.trimmed();
    return c;
}

Column* Table::foreignKey(const QString &foreignKeyCol, const QString &referenceTable, const QString &referenceCol, const QString &constraintSymbol, const QString &indexName)
{
    return foreignKey(QStringList(foreignKeyCol), referenceTable, QStringList(referenceCol), constraintSymbol, indexName);
}

#include "moc_table.cpp"
