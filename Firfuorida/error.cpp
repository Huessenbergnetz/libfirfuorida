/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "error_p.h"
#include <QDebug>

using namespace Firfuorida;

Error::Error() :
    d(new ErrorData)
{

}

Error::Error(ErrorType type, const QString &text) :
    d(new ErrorData(type, text))
{

}

Error::Error(const QSqlError &sqlError, const QString &text) :
    d(new ErrorData(sqlError, text))
{

}

Error::Error(const Error &other) = default;

Error::Error(Error &&error) noexcept = default;

Error& Error::operator=(const Error &other) = default;

Error& Error::operator=(Error &&ohter) noexcept = default;

Error::~Error() = default;

void Error::swap(Error &other) noexcept
{
    std::swap(d, other.d);
}

Error::ErrorType Error::type() const
{
    return d->type;
}

QString Error::text() const
{
    if (d->type == SqlError) {
        return d->text + QChar(QChar::Space) + d->sqlError.text();
    } else {
        return d->text;
    }
}

QSqlError Error::sqlError() const
{
    return d->sqlError;
}

bool Error::operator==(const Error &other) const noexcept
{
    if (type() != other.type()) {
        return false;
    }
    if (text() != other.text()) {
        return false;
    }
    if (sqlError() != other.sqlError()) {
        return false;
    }
    return true;
}

QDebug operator<<(QDebug dbg, const Firfuorida::Error &error)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << error.text();;
    return dbg.maybeSpace();
}
