/*\\ß\
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MIGRATOR_H
#define MIGRATOR_H

#include "firfuorida_global.h"
#include <QObject>
#include <QSqlDatabase>

namespace Firfuorida {

class MigratorPrivate;

/*!
 * \brief Manages multiple migrations.
 *
 * The %Migrator class manages multiple Migration objects that are children of the
 * %Migrator class.
 *
 * <h2>Example</h2>
 * example.h
 * \include examplemigration.h
 *
 * example.cpp
 * \include examplemigration.cpp
 *
 * main.cpp
 * \include examplemigratormain.cpp
 *
 * \headerfile "" <Firfuorida/Migrator>
 */
class FIRFUORIDA_LIBRARY Migrator : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Migrator)
    const QScopedPointer<MigratorPrivate> dptr;
    Q_DECLARE_PRIVATE_D(dptr, Migrator)
public:
    /*!
     * \brief Constructs a new %Migrator object with default values and given \a parent.
     *
     * Uses QSqlDatabase::defaultConnection as \a connectionName and \c "migrations" as
     * \a migrationsTable.
     */
    explicit Migrator(QObject *parent = nullptr);
    /*!
     * \brief Constructs a new %Migrator object with the given parameters.
     * \param connectionName  The name of the SQL connection to use.
     * \param migrationsTable The name of the table to store the migrations in.
     * \param parent          Pointer to a parent object.
     */
    explicit Migrator(const QString &connectionName, const QString &migrationsTable, QObject *parent = nullptr);
    /*!
     * \brief Deconstructs the %Migrator object.
     */
    ~Migrator();

    enum DatabaseType : uint8_t {
        Invalid     = 0,
        DB2         = 1,
        InterBase   = 2,
        MySQL       = 3,
        MariaDB     = 5,
        ODBC        = 6,
        OCI         = 7,
        PSQL        = 8,
        SQLite      = 9
    };

    /*!
     * \brief Returns the name of the used SQL connection.
     */
    QString connectionName() const;
    /*!
     * \brief Returns the name of the table to store migrations to.
     */
    QString migrationsTable() const;

    /*!
     * \brief Runs all migrations not already applied and return \c true on success.
     */
    bool migrate();
    /*!
     * \brief Rolls back the number of migrations set by \a steps and returns \c true on success.
     *
     * If \a steps is \c 0, only one migration will be rolled back.
     */
    bool rollback(uint steps = 1);
    /*!
     * \brief Rolls back all migrations and returns \c true on success.
     */
    bool reset();
    /*!
     * \brief Rolls back and reapplies the number of migrations defined by \a steps.
     *
     * If \a steps is \c 0, all migrations are rolled back and reapplied.
     */
    bool refresh(uint steps = 0);
};

}

#endif // MIGRATOR_H
