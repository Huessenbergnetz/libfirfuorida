/*\\ß\
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MIGRATOR_H
#define MIGRATOR_H

#include "firfuorida_global.h"
#include <QObject>
#include <QSqlDatabase>
#include <QVersionNumber>
#include <QFlags>

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

    /*!
     * \brief These are the database systems supported by default by Qt.
     */
    enum DatabaseType : uint8_t {
        Invalid     = 0,    /**< No valid database diver/type has been set. */
        DB2         = 1,    /**< IBM DB2 (version 7.1 and above) */
        InterBase   = 2,    /**< Borland InterBase */
        MySQL       = 3,    /**< MySQL */
        MariaDB     = 5,    /**< MariaDB (version 5.0 and above) */
        ODBC        = 6,    /**< Open Database Connectivity (ODBC) - Microsoft SQL Server and other ODBC-compliant databases */
        OCI         = 7,    /**< Oracle Call Interface Driver */
        PSQL        = 8,    /**< PostgreSQL (versions 7.3 and above) */
        SQLite      = 9     /**< SQLite version 3 */
    };
    Q_ENUM(DatabaseType)

    /*!
     * \brief Features supported by the currently used database system.
     * \sa dbFeatures(), isDbFeatureAvailable()
     */
    enum DatabaseFeature : int {
        NoFeatures          = 1 << 0, /**< No special features are available. */
        DefValOnText        = 1 << 1, /**< Supports default values on text type columns. */
        DefValOnBlob        = 1 << 2, /**< Supports default values on blob type columns. */
        DefValOnGeometry    = 1 << 3, /**< Supports default values on geometry type columns. */
        JSONTypes           = 1 << 4, /**< Supports JSON data types. */
        GeometryTypes       = 1 << 5, /**< Supports geometry data types. */
        XMLType             = 1 << 6, /**< Supports XML data types. */
        NetworkAddressTypes = 1 << 7, /**< Supports network address data types. */
        MonetaryTypes       = 1 << 8, /**< Supports monetary data types. */
        ForeignKeys         = 1 << 9, /**< Supports foreign key constraints. */
        CommentsOnColumns   = 1 << 10, /**< Supports comments on columns. */
        CommentsOnTables    = 1 << 11, /**< Supports comments on tables. */
        SetType             = 1 << 12, /**< Supports SET data type. */
        EnumType            = 1 << 13  /**< Supports ENUM data type. */
    };
    Q_DECLARE_FLAGS(DatabaseFeatures, DatabaseFeature)
    Q_FLAGS(DatabaseFeatures)

    /*!
     * \brief Opens and initializes the database.
     *
     * If the databae is not already open, this will try to open it. After that it will set
     * the dbType(), the dbVersion() and the dbFeatures(). This has not to be called explicitely
     * but is called by migrate() and rollback() automatically.
     */
    bool initDatabase();

    /*!
     * \brief Returns the type of the used database system.
     *
     * \note This is only availbale after initDatabase(), migrate() or rollback() has been called.
     */
    DatabaseType dbType() const;

    /*!
     * \brief Returns the name of the used database system.
     *
     * \note This is only availbale after initDatabase(), migrate() or rollback() has been called.
     */
    QString dbTypeToStr() const;

    /*!
     * \brief Returns the version of the used database system.
     *
     * \note This is only availbale after initDatabase(), migrate() or rollback() has been called.
     */
    QVersionNumber dbVersion() const;

    /*!
     * \brief Returns features supported by the used datbase system.
     *
     * \note This is only availbale after initDatabase(), migrate() or rollback() has been called.
     */
    DatabaseFeatures dbFeatures() const;

    /*!
     * \brief Returns \c true if the \a dbFeatures are available on the used database system.
     *
     * \note This is only availbale after initDatabase(), migrate() or rollback() has been called.
     */
    bool isDbFeatureAvailable(DatabaseFeatures dbFeatures) const;

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

Q_DECLARE_OPERATORS_FOR_FLAGS(Firfuorida::Migrator::DatabaseFeatures)

#endif // MIGRATOR_H
