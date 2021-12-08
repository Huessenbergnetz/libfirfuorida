/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MIGRATIONTESTOBJECT_H
#define MIGRATIONTESTOBJECT_H

#include <QObject>
#include <QProcess>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QVariant>

class MigrationTestObject : public QObject
{
    Q_OBJECT
public:
    explicit MigrationTestObject(QObject *parent = nullptr);
    ~MigrationTestObject() override;

    enum ColOpt {
        NoOptions = 0,
        Nullable = 1,
        PrimaryKey = 2,
        AutoIncrement = 4,
        Unsigned = 8,
        Unique = 16
    };
    Q_DECLARE_FLAGS(ColOpts, ColOpt)

protected:
    bool startMysql();
    bool stopMysql();
    bool createDatabase(const QString &name, const QString &user, const QString &password);

    const int m_mysqlPort{46000};
    QString mysqlSocketPath() const;

    bool tableExists(const QString &tableName) const;

    bool checkTableEngine(const QString &tableName, const QString &engine) const;

    bool checkColumn(const QString &table, const QString &column, const QString &type, ColOpts options = NoOptions, const QVariant &defVal = QVariant()) const;

private:
    QTemporaryDir m_mysqlWorkingDir;
    QTemporaryDir m_mysqlDataDir;
    QTemporaryDir m_mysqlSocketDir;
    QTemporaryDir m_mysqlLogDir;
    QTemporaryFile m_mysqlConfigFile;
    QProcess m_mysqlProcess;
    QString m_dbRootConn;
    QString m_dbName;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MigrationTestObject::ColOpts)

#endif // MIGRATIONTESTOBJECT_H
