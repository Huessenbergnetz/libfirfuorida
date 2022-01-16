/*
 * SPDX-FileCopyrightText: (C) 2019-2021 Matthias Fehring / www.huessenbergnetz.de
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "migrationtestobject.h"
#include "../Firfuorida/migration.h"
#include "../Firfuorida/migrator.h"
#include <QTest>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>

#define DB_NAME "migtestdb"
#define DB_USER "migtester"
#define DB_PASS "lalalala1"
#define DB_CONN "migtests"

class M20190121T174100_Tiny : public Firfuorida::Migration
{
    Q_OBJECT
public:
    explicit M20190121T174100_Tiny(Firfuorida::Migrator *parent) : Firfuorida::Migration(parent) {}
    ~M20190121T174100_Tiny() override {}

    void up() override
    {
        auto t = create(QStringLiteral("tiny"));
        t->tinyIncrements();
        t->tinyInteger(QStringLiteral("tinyIntCol"))->unique();
        t->tinyBlob(QStringLiteral("tinyBlobCol"))->nullable();
        t->tinyText(QStringLiteral("tinyTextCol"))->defaultValue(QStringLiteral("dummer schiss"));
    }

    void down() override
    {
        drop(QStringLiteral("tiny"));
    }
};

class M20190122T101200_Small : public Firfuorida::Migration
{
    Q_OBJECT
public:
    explicit M20190122T101200_Small(Firfuorida::Migrator *parent) : Firfuorida::Migration(parent) {}
    ~M20190122T101200_Small() override {}

    void up() override
    {
        auto t = create(QStringLiteral("small"));
        t->smallIncrements();
        t->smallInteger(QStringLiteral("smallIntCol"));
        t->blob(QStringLiteral("blobCol"));
        t->text(QStringLiteral("textCol"));
    }

    void down() override
    {
        drop(QStringLiteral("small"));
    }
};

class M20190125T113700_Medium : public Firfuorida::Migration
{
    Q_OBJECT
public:
    explicit M20190125T113700_Medium(Firfuorida::Migrator *parent) : Firfuorida::Migration(parent) {}
    ~M20190125T113700_Medium() override {}

    void up() override
    {
        auto t = create(QStringLiteral("medium"));
        t->increments();
        t->integer(QStringLiteral("intCol"));
        t->mediumBlob(QStringLiteral("mediumBlobCol"));
        t->mediumText(QStringLiteral("mediumTextCol"));
    }

    void down() override
    {
        drop(QStringLiteral("medium"));
    }
};

class M20190125T141000_Big : public Firfuorida::Migration
{
    Q_OBJECT
public:
    explicit M20190125T141000_Big(Firfuorida::Migrator *parent) : Firfuorida::Migration(parent) {}
    ~M20190125T141000_Big() override {}

    void up() override
    {
        auto t = create(QStringLiteral("big"));
        t->bigIncrements();
        t->bigInteger(QStringLiteral("bigIntCol"));
        t->longBlob(QStringLiteral("longBlobCol"));
        t->longText(QStringLiteral("longTextCol"));
    }

    void down() override
    {
        drop(QStringLiteral("big"));
    }
};

class MigrationTest : public MigrationTestObject
{
    Q_OBJECT
public:
    explicit MigrationTest(QObject *parent = nullptr) : MigrationTestObject(parent) {}

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testTinyCols();
    void testMigration();

private:
    Firfuorida::Migrator *m_testmigrator = nullptr;
};

void MigrationTest::initTestCase()
{
    if (!startMysql()) {
        QFAIL("Failed to start MySQL.");
    }
    if (!createDatabase(QStringLiteral(DB_NAME), QStringLiteral(DB_USER), QStringLiteral(DB_PASS))) {
        QFAIL("Failed to create database.");
    }

    m_testmigrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20190121T174100_Tiny(m_testmigrator);
    new M20190122T101200_Small(m_testmigrator);
    new M20190125T113700_Medium(m_testmigrator);
    new M20190125T141000_Big(m_testmigrator);

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QMYSQL"), QStringLiteral(DB_CONN));
    db.setDatabaseName(QStringLiteral(DB_NAME));
    db.setUserName(QStringLiteral(DB_USER));
    db.setPassword(QStringLiteral(DB_PASS));
    db.setHostName(QStringLiteral("127.0.0.1"));
    db.setPort(m_mysqlPort);
    if (!db.open()) {
        qDebug() << db.lastError().text();
        QFAIL("Failed to establish database connection.");
    }
}

void MigrationTest::cleanupTestCase()
{
    QSqlDatabase::removeDatabase(QStringLiteral(DB_CONN));
}

void MigrationTest::testTinyCols()
{
    auto migrator = new Firfuorida::Migrator(QStringLiteral(DB_CONN), QStringLiteral("migrations"), this);
    new M20190121T174100_Tiny(migrator);
    QVERIFY(migrator->migrate());
    QVERIFY(tableExists(QStringLiteral("tiny")));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("id"), QStringLiteral("tinyint"), MigrationTestObject::PrimaryKey|MigrationTestObject::AutoIncrement|MigrationTestObject::Unsigned));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyIntCol"), QStringLiteral("tinyint"), MigrationTestObject::Unique));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyBlobCol"), QStringLiteral("tinyblob"), MigrationTestObject::Nullable));
    if (dbSupportsDefValOnTextAndBlob()) {
        QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("tinytext"), MigrationTestObject::NoOptions, QStringLiteral("dummer schiss")));
    } else {
        QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("tinytext"), MigrationTestObject::NoOptions));
    }
    QVERIFY(migrator->rollback());
    QVERIFY(!tableExists(QStringLiteral("tiny")));
}

void MigrationTest::testMigration()
{
    QVERIFY(m_testmigrator->migrate());
    QVERIFY(tableExists(QStringLiteral("tiny")));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("id"), QStringLiteral("tinyint"), MigrationTestObject::PrimaryKey|MigrationTestObject::AutoIncrement|MigrationTestObject::Unsigned));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyIntCol"), QStringLiteral("tinyint"), MigrationTestObject::Unique));
    QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyBlobCol"), QStringLiteral("tinyblob"), MigrationTestObject::Nullable));
    if (dbSupportsDefValOnTextAndBlob()) {
        QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("tinytext"), MigrationTestObject::NoOptions, QStringLiteral("dummer schiss")));
    } else {
        QVERIFY(checkColumn(QStringLiteral("tiny"), QStringLiteral("tinyTextCol"), QStringLiteral("tinytext"), MigrationTestObject::NoOptions));
    }
    QVERIFY(tableExists(QStringLiteral("small")));
    QVERIFY(checkColumn(QStringLiteral("small"), QStringLiteral("id"), QStringLiteral("smallint"), MigrationTestObject::PrimaryKey|MigrationTestObject::AutoIncrement|MigrationTestObject::Unsigned));
    QVERIFY(tableExists(QStringLiteral("medium")));
    QVERIFY(checkColumn(QStringLiteral("medium"), QStringLiteral("id"), QStringLiteral("int"), MigrationTestObject::PrimaryKey|MigrationTestObject::AutoIncrement|MigrationTestObject::Unsigned));
    QVERIFY(tableExists(QStringLiteral("big")));
    QVERIFY(checkColumn(QStringLiteral("big"), QStringLiteral("id"), QStringLiteral("bigint"), MigrationTestObject::PrimaryKey|MigrationTestObject::AutoIncrement|MigrationTestObject::Unsigned));
//    QVERIFY(m_testmigrator->rollback(1));
//    QVERIFY(!tableExists(QStringLiteral("small")));
//    QVERIFY(tableExists(QStringLiteral("tiny")));
//    QVERIFY(m_testmigrator->rollback());
//    QVERIFY(!tableExists(QStringLiteral("small")));
//    QVERIFY(!tableExists(QStringLiteral("tiny")));
}

QTEST_MAIN(MigrationTest)

#include "testmigration.moc"
