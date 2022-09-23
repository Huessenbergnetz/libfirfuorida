/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_ERROR_P_H
#define FIRFUORIDA_ERROR_P_H

#include "error.h"
#include <QSharedData>

namespace Firfuorida {

class ErrorData : public QSharedData
{
public:
    ErrorData() : QSharedData() {}

    ErrorData(Error::ErrorType _type, const QString &_text) :
        QSharedData(),
        text(_text),
        type(_type)
    {}

    ErrorData(const QSqlError &_sqlError, const QString &_text) :
        QSharedData(),
        sqlError(_sqlError),
        text(_text)
    {
        type = Error::SqlError;
    }

    ~ErrorData() {}

    QSqlError sqlError;
    QString text;
    Error::ErrorType type = Error::NoError;
};

}

#endif // FIRFUORIDA_ERROR_P_H
