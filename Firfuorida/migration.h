/*0
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FIRFUORIDA_MIGRATION_H
#define FIRFUORIDA_MIGRATION_H

#include "firfuorida_global.h"
#include "table.h"
#include "migrator.h"
#include "error.h"
#include <QObject>
#include <functional>

namespace Firfuorida {

class MigrationPrivate;

/*!
 * \brief Contains a single migration instance.
 *
 * The %Migration object represents a single migration instance. A migration is defined by
 * operations defined in the reimplemented up() and down() functions. The reimplemented up()
 * function is called when the migration is done. The reimplemented down() function is called
 * when the migrations will be rolled back. Inside up() and down() use the create(), createTableIfNotExists(),
 * table(), drop(), dropIfExists(), rename() and raw() functions.
 *
 * Additionally there is the possibility to use a custom function for migration by reimplenting
 * executeUp() and executeDown().
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
 * \headerfile "" <Firfuorida/Migration>
 */
class FIRFUORIDA_LIBRARY Migration : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Migration)
    friend class Migrator;
    const QScopedPointer<MigrationPrivate> dptr;
    Q_DECLARE_PRIVATE_D(dptr, Migration)
public:
    /*!
     * \brief Constructs a new %Migration object with the given \a parent.
     *
     * The \a parent has to be a valid Migrator object.
     */
    explicit Migration(Migrator *parent);
    /*!
     * \brief Deconstructs the %Migration object.
     */
    ~Migration() override;

protected:
    /*!
     * \brief Reimplement this function to perform database operations when performing migrations.
     *
     * <h3>Example</h3>
     * \code{.cpp}
     * void M20190121T174100_Example::up()
     * {
     *     // create a new table named "ExampleTable" if it not already exists
     *     auto t = createTableIfNotExists(QStringLiteral("ExampleTable"));
     *     // create a new unsigned tiny integer column on that table with the
     *     // default name "id" and auto incrementation enabled
     *     t->tinyIncrements();
     *     // create a new signed tiny integer column on that table with the
     *     // name "tinyIntCol" and UNIQUE KEY index.
     *     t->tinyInteger(QStringLiteral("tinyIntCol"))->unique();
     *     // create a new nullable tiny blob column on that table with the name
     *     // "tinyBlobCol"
     *     t->tinyBlob(QStringLiteral("tinyBlobCol"))->nullable();
     *     // create a new tiny text column on that table with the name "tinyTextCol"
     *     // and the default value "foobar"
     *     t->tinyText(QStringLiteral("tinyTextCol"))->defaultValue(QStringLiteral("foobar"));
     * }
     * \endcode
     * \sa down()
     */
    virtual void up() = 0;
    /*!
     * \brief Reimplement this function to perform database operations when performing migration rollback.
     *
     * <h3>Example</h3>
     * \code{.cpp}
     * void M20190121T174100_Example::down()
     * {
     *     // drop the table named "ExampleTable" if it exists
     *     dropIfExists(QStringLiteral("ExampleTable"));
     * }
     * \endcode
     * \sa up()
     */
    virtual void down() = 0;
    virtual bool executeUp();
    virtual bool executeDown();
    /*!
     * \brief Creates a new table with the given \a tableName.
     *
     * The \a tableName is not allowed to be empty. Use the returned Table
     * object to create columns on it.
     *
     * \sa createTableIfNotExists()
     */
    Table* create(const QString &tableName);
    /*!
     * \brief Creates a new table with the given \a tableName if it not exists already.
     *
     * The \a tableName is not allowed to be empty. Use the returned Table
     * object to create columns on it.
     *
     * \sa create()
     */
    Table* createTableIfNotExists(const QString &tableName);
    /*!
     * \brief Returns an existing Table object for a table with the given \a tableName.
     *
     * The \a tableName is not allowed to be empty. Use the returned Table
     * object to modify or crate columns on it.
     */
    Table* table(const QString &tableName);
    /*!
     * \brief Drops the table identified by \a tableName.
     *
     * \sa dropIfExists()
     */
    void drop(const QString &tableName);
    /*!
     * \brief Drops the table identified by \a tableName if it exists.
     *
     * \sa drop()
     */
    void dropIfExists(const QString &tableName);
    /*!
     * \brief Renames the table named \a tableName to \a newName.
     */
    void rename(const QString &tableName, const QString &newName);
    /*!
     * \brief Executes a raw SQL \a statement.
     */
    void raw(const QString &statement);
    void executeUpFunction();
    void executeDownFunction();

    /*!
     * \brief Returns the type of the used database system.
     *
     * \note This is only availbale after Migrator::initDatabase(), Migrator::migrate()
     * or Migrator::rollback() has been called.
     */
    Migrator::DatabaseType dbType() const;
    /*!
     * \brief Returns the name of the used database system.
     *
     * \note This is only availbale after Migrator::initDatabase(), Migrator::migrate()
     * or Migrator::rollback() has been called.
     */
    QString dbTypeToStr() const;
    /*!
     * \brief Returns the version of the used database system.
     *
     * \note This is only availbale after Migrator::initDatabase(), Migrator::migrate()
     * or Migrator::rollback() has been called.
     */
    QVersionNumber dbVersion() const;
    /*!
     * \brief Returns features supported by the used datbase system.
     *
     * \note This is only availbale after Migrator::initDatabase(), Migrator::migrate()
     * or Migrator::rollback() has been called.
     */
    Migrator::DatabaseFeatures dbFeatures() const;
    /*!
     * \brief Returns \c true if the \a dbFeatures are available on the used database system.
     *
     * \note This is only availbale after Migrator::initDatabase(), Migrator::migrate()
     * or Migrator::rollback() has been called.
     */
    bool isDbFeatureAvailable(Migrator::DatabaseFeatures dbFeatures) const;

    /*!
     * \brief Returns error information about the last error (if any) that occurred with this migrator.
     */
    Error lastError() const;
};

}

#endif // FIRFUORIDA_MIGRATION_H
