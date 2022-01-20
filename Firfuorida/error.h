/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_ERROR_H
#define FIRFUORIDA_ERROR_H

#include "firfuorida_global.h"
#include <QString>
#include <QSharedDataPointer>
#include <QSqlError>

namespace Firfuorida {

class ErrorData;

/*!
 * \brief Provides error information.
 *
 * The %Error class provides information about occured errors. Use type()
 * to check if an error has occure. text() will return the error text. If
 * the error is an SqlError, there will also be a valid QSqlError returned
 * by sqlError().
 *
 * \headerfile "" <Firfuorida/Error>
 */
class FIRFUORIDA_LIBRARY Error
{
public:
    /*!
     * \brief This enum type describes the context in which the error occurred.
     */
    enum ErrorType : uint8_t {
        NoError         = 0,    /**< No error occured. */
        SqlError        = 1,    /**< A SQL error occured, sqlError() will return a valid QSqlError object. */
        FileSystemError = 2,    /**< A filesystem error occured. */
        InternalError   = 3,    /**< An internal error occured. */
        UnknownError    = 255   /**< An unknown error occured. */
    };

    /*!
     * \brief Constructs a new %Error object of type NoError.
     */
    Error();

    /*!
     * \brief Constructs a new %Error object with given \a type and \a text.
     */
    Error(ErrorType type, const QString &text);

    /*!
     * \brief Constucts a new %Error object with given \a sqlError and \a text.
     *
     * The type() will automatically set to SqlError.
     */
    Error(const QSqlError &sqlError, const QString &text);

    /*!
     * \brief Constructs a copy of \a other.
     */
    Error(const Error &other);
    /*!
     * \brief Move-constructs an %Error instance, making it point at the same object that \a other was pointing to.
     */
    Error(Error &&other) noexcept;
    /*!
     * \brief Assigns \a other to this %Error and returns a reference to this instance.
     */
    Error& operator=(const Error &other);
    /*!
     * \brief Move-assigns \a other to this %Error instance.
     */
    Error& operator=(Error &&other) noexcept;

    /*!
     * \brief Deconstructs the %Error object.
     */
    ~Error();

    /*!
     * \brief Swaps this %Error instance with \a other.
     */
    void swap(Error &other) noexcept;

    /*!
     * \brief Returns the type of this error.
     */
    ErrorType type() const;
    /*!
     * \brief Returns the error text.
     *
     * If error is of type() SqlError, the text will also contain the text
     * returned by QSqlError::text();
     */
    QString text() const;
    /*!
     * \brief Returns the QSqlError object if this error is of type SqlError.
     */
    QSqlError sqlError() const;

    /*!
     * \brief Returns \c true if \a this and \a other have the same content; otherwise returns \c false.
     */
    bool operator==(const Error &other) const noexcept;
    /*!
     * \brief Returns \c true if \a this and \a other have not the same content; otherwise returns \c false.
     */
    inline bool operator!=(const Error &other) const noexcept { return !operator==(other); }

protected:
    QSharedDataPointer<ErrorData> d;
};

}

/*!
 * \relates Firfuorida::Error
 * \brief Writes the text() of \a error to the \a dbg stream and returns the stream.
 */
FIRFUORIDA_LIBRARY QDebug operator<<(QDebug dbg, const Firfuorida::Error &error);

#endif // FIRFUORIDA_ERROR_H
