/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "migrationtestobject.h"
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

MigrationTestObject::MigrationTestObject(QObject *parent) : QObject(parent)
{

}

MigrationTestObject::~MigrationTestObject()
{

}

bool MigrationTestObject::startMysql()
{
    if (!m_mysqlWorkingDir.isValid()) {
        qCritical() << "Can not create mysql working directory.";
        return false;
    }

    if (!m_mysqlDataDir.isValid()) {
        qCritical() << "Can not create mysql data directory.";
        return false;
    }

    if (!m_mysqlSocketDir.isValid()) {
        qCritical() << "Can not create mysql socket directory.";
        return false;
    }

    if (!m_mysqlLogDir.isValid()) {
        qCritical() << "Can not create mysql log directory.";
        return false;
    }

    if (!m_mysqlConfigFile.open()) {
        qCritical() << "Can not open mysql config file.";
        return false;
    }

    {
        QTextStream mysqlConfOut(&m_mysqlConfigFile);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        mysqlConfOut.setCodec("UTF-8");
#endif
        mysqlConfOut << "[mysqld]" << '\n';
        mysqlConfOut << "bind-address=127.0.0.1" << '\n';
        mysqlConfOut << "log-error=" << m_mysqlLogDir.filePath(QStringLiteral("mysql.log")) << '\n';
        mysqlConfOut << "innodb_file_format=Barracuda" << '\n';
        mysqlConfOut << "innodb_file_per_table=ON" << '\n';
        mysqlConfOut << "datadir=" << m_mysqlDataDir.path() << '\n';
        mysqlConfOut << "port=" << m_mysqlPort << '\n';
        mysqlConfOut << "socket=" << m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock")) << '\n';
        mysqlConfOut << "server-id=1" << '\n';
        mysqlConfOut.flush();
    }

    const QString mysqlDefaultsArg = QStringLiteral("--defaults-file=") + m_mysqlConfigFile.fileName();
    const QStringList mysqlInstallArgs{QStringLiteral("--force"), QStringLiteral("--auth-root-authentication-method=normal"), mysqlDefaultsArg};

    if (QProcess::execute(QStringLiteral("/usr/bin/mysql_install_db"), mysqlInstallArgs) != 0) {
        qCritical() << "Can not install initial mysql database.";
        return false;
    }

    QFileInfo socketFi(m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock")));

    m_mysqlProcess.setArguments({mysqlDefaultsArg});
    m_mysqlProcess.setProgram(QStringLiteral("/usr/sbin/mysqld"));
    m_mysqlProcess.setWorkingDirectory(m_mysqlWorkingDir.path());
    m_mysqlProcess.start();

    auto cur = QDateTime::currentDateTime();
    while (!socketFi.exists() && (cur.msecsTo(QDateTime::currentDateTime()) < 5000)) {

    }

    if (!socketFi.exists()) {
        qCritical() << "Failed to start mysql server within 5 seconds.";
        qDebug() << "MySQL startup params:" << m_mysqlProcess.arguments().join(QChar(QChar::Space));
        return false;
    }

    return true;
}

bool MigrationTestObject::stopMysql()
{
    m_mysqlProcess.terminate();
    return m_mysqlProcess.exitCode() == 0;
}

bool MigrationTestObject::createDatabase(const QString &name, const QString &user, const QString &password)
{
    const QString initConName = name + QStringLiteral("-initCon");
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), initConName);
        db.setDatabaseName(QStringLiteral("mysql"));
        db.setUserName(QStringLiteral("root"));
        db.setConnectOptions(QStringLiteral("UNIX_SOCKET=%1").arg(m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock"))));
        if (!db.open()) {
            qCritical() << "Failed to establish database connection:" << db.lastError().text();
            return false;
        }

        QSqlQuery q(db);

        if (!q.exec(QStringLiteral("CREATE DATABASE %1").arg(name))) {
            qCritical() << "Failed to create database:" << q.lastError().text();
            return false;
        }

        if (!q.exec(QStringLiteral("CREATE USER '%1'@'localhost' IDENTIFIED BY '%2'").arg(user, password))) {
            qCritical() << "Failed to create database user:" << q.lastError().text();
            return false;
        }

        if (!q.exec(QStringLiteral("GRANT ALL ON %1.* TO '%2'@'localhost'").arg(name, user))) {
            qCritical() << "Failed to grant access to db user:" << q.lastError().text();
            return false;
        }

        db.close();
        m_dbName = name;
    }

    QSqlDatabase::removeDatabase(initConName);

    {
        m_dbRootConn = name + QStringLiteral("-rootCon");
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), m_dbRootConn);
        db.setDatabaseName(QStringLiteral("information_schema"));
        db.setUserName(QStringLiteral("root"));
        db.setConnectOptions(QStringLiteral("UNIX_SOCKET=%1").arg(m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock"))));
        if (!db.open()) {
            qCritical() << "Failed to establish database connection:" << db.lastError().text();
            return false;
        }
    }

    return true;
}

QString MigrationTestObject::mysqlSocketPath() const
{
    return m_mysqlSocketDir.filePath(QStringLiteral("mysql.sock"));
}

bool MigrationTestObject::tableExists(const QString &tableName) const
{
    QSqlQuery q(QSqlDatabase::database(m_dbRootConn));
    if (q.prepare(QStringLiteral("SELECT COUNT(*) FROM TABLES WHERE TABLE_SCHEMA = :db AND TABLE_NAME = :tableName"))) {
        q.bindValue(QStringLiteral(":db"), m_dbName);
        q.bindValue(QStringLiteral(":tableName"), tableName);
        if (q.exec() && q.next() && q.value(0).toInt() == 1) {
            return true;
        }
    }

    return false;
}

bool MigrationTestObject::checkTableEngine(const QString &tableName, const QString &engine) const
{
    QSqlQuery q(QSqlDatabase::database(m_dbRootConn));
    if (q.prepare(QStringLiteral("SELECT ENGINE FROM TABLES WHERE TABLE_SCHEMA = :db AND TABLE_NAME = :tableName"))) {
        q.bindValue(QStringLiteral(":db"), m_dbName);
        q.bindValue(QStringLiteral(":tableName"), tableName);
        if (q.exec() && q.next()) {
            return q.value(0).toString() == engine;
        }
    }

    return false;
}

bool MigrationTestObject::checkColumn(const QString &table, const QString &column, const QString &type, ColOpts options, const QVariant &defVal) const
{
    QSqlQuery q(QSqlDatabase::database(m_dbRootConn));
    if (q.prepare(QStringLiteral("SELECT DATA_TYPE, IS_NULLABLE, COLUMN_KEY, EXTRA, COLUMN_TYPE, COLUMN_DEFAULT FROM COLUMNS WHERE TABLE_SCHEMA = :db AND TABLE_NAME = :table AND COLUMN_NAME = :column"))) {
        q.bindValue(QStringLiteral(":db"), m_dbName);
        q.bindValue(QStringLiteral(":table"), table);
        q.bindValue(QStringLiteral(":column"), column);
        if (q.exec() && q.next()) {

            if (q.value(0).toString().toLower() != type.toLower()) {
                return false;
            }

            const QString _nullable = options.testFlag(Nullable) ? QStringLiteral("yes") : QStringLiteral("no");
            if (q.value(1).toString().toLower() != _nullable) {
                return false;
            }

            const auto _colKey = q.value(2).toString();
            if (options.testFlag(PrimaryKey) && !_colKey.contains(QLatin1String("PRI"), Qt::CaseInsensitive)) {
                return false;
            } else if (!options.testFlag(PrimaryKey) && _colKey.contains(QLatin1String("PRI"), Qt::CaseInsensitive)) {
                return false;
            }
            if (options.testFlag(Unique) && !_colKey.contains(QLatin1String("UNI"), Qt::CaseInsensitive)) {
                return false;
            } else if (!options.testFlag(Unique) && _colKey.contains(QLatin1String("UNI"), Qt::CaseInsensitive)) {
                return false;
            }

            const auto _extra = q.value(3).toString();
            if (options.testFlag(AutoIncrement) && !_extra.contains(QLatin1String("auto_increment"), Qt::CaseInsensitive)) {
                return false;
            } else if (!options.testFlag(AutoIncrement) && _extra.contains(QLatin1String("auto_increment"), Qt::CaseInsensitive)) {
                return false;
            }

            const auto _colType = q.value(4).toString();
            if (options.testFlag(Unsigned) && !_colType.contains(QLatin1String("unsigned"), Qt::CaseInsensitive)) {
                return false;
            } else if (!options.testFlag(Unsigned) && _colType.contains(QLatin1String("unsigned"), Qt::CaseInsensitive)) {
                return false;
            }

            if (defVal.isValid()) {
                auto _colDef = q.value(5).toString();
                const auto _defVal = defVal.toString();
                if (_colDef.startsWith(QLatin1Char('\'')) && _colDef.endsWith(QLatin1Char('\''))) {
                    _colDef.chop(1);
                    _colDef.remove(0,1);
                }
                if (_colDef != _defVal) {
                    qCritical("Unexpected default value: %s != %s", qUtf8Printable(_colDef), qUtf8Printable(_defVal));
                    return false;
                }
            }

            return true;
        }
    }

    return false;
}
