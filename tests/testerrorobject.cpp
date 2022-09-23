/*
 * SPDX-FileCopyrightText: (C) 2019-2022 Matthias Fehring <https://www.huessenbergnetz.de>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "../Firfuorida/error.h"
#include <QObject>
#include <QTest>

class TestErrorObject : public QObject
{
    Q_OBJECT
public:
    TestErrorObject(QObject *parent = nullptr) : QObject(parent) {}
    ~TestErrorObject() override {}

private Q_SLOTS:
    void testDefaultConstructor();
    void testConstructorWithArgs();
    void testCompare();
    void testMove();
};

void TestErrorObject::testDefaultConstructor()
{
    Firfuorida::Error e;
    QCOMPARE(e.type(), Firfuorida::Error::NoError);
    QVERIFY(e.text().isEmpty());
    QCOMPARE(e.sqlError().type(), QSqlError::NoError);
}

void TestErrorObject::testConstructorWithArgs()
{
    const QString e1Text = QStringLiteral("Can not open file");
    Firfuorida::Error e1(Firfuorida::Error::FileSystemError, e1Text);
    QCOMPARE(e1.type(), Firfuorida::Error::FileSystemError);
    QCOMPARE(e1.text(), e1Text);
    QCOMPARE(e1.sqlError().type(), QSqlError::NoError);

    QSqlError sqlError(QStringLiteral("Drivertext"), QStringLiteral("database text"), QSqlError::StatementError);
    const QString e2Text = QStringLiteral("Can not execute database query statement.");
    const QString e2CompText = e2Text + QChar(QChar::Space) + QStringLiteral("database text") + QChar(QChar::Space) + QStringLiteral("Drivertext");
    Firfuorida::Error e2(sqlError, e2Text);
    QCOMPARE(e2.type(), Firfuorida::Error::SqlError);
    QCOMPARE(e2.text(), e2CompText);
    QCOMPARE(e2.sqlError(), sqlError);
}

void TestErrorObject::testCompare()
{
    const Firfuorida::Error e1(Firfuorida::Error::FileSystemError, QStringLiteral("Can not open file."));
    const Firfuorida::Error e2(Firfuorida::Error::FileSystemError, QStringLiteral("Can not open file."));
    const Firfuorida::Error e3 = e1;
    const Firfuorida::Error e4(Firfuorida::Error::UnknownError, QStringLiteral("An unknown error occure."));
    const Firfuorida::Error e5;
    const Firfuorida::Error e6;

    QVERIFY(e1 == e2);
    QVERIFY(e1 == e3);
    QVERIFY(e1 != e4);
    QVERIFY(e2 != e4);
    QVERIFY(e3 != e4);
    QVERIFY(e4 != e5);
    QVERIFY(e5 != e4);
    QVERIFY(e5 == e6);
}

void TestErrorObject::testMove()
{
    // test move constructor
    {
        Firfuorida::Error e1(Firfuorida::Error::FileSystemError, QStringLiteral("Can not open file."));
        Firfuorida::Error e2(std::move(e1));
        QCOMPARE(e2.type(), Firfuorida::Error::FileSystemError);
        QCOMPARE(e2.text(), QStringLiteral("Can not open file."));
        QCOMPARE(e2.sqlError(), QSqlError());
    }

    // test move assignment
    {
        Firfuorida::Error e1(Firfuorida::Error::FileSystemError, QStringLiteral("Can not open file."));
        Firfuorida::Error e2(Firfuorida::Error::UnknownError, QStringLiteral("An unknown error occure."));
        e2 = std::move(e1);
        QCOMPARE(e2.type(), Firfuorida::Error::FileSystemError);
        QCOMPARE(e2.text(), QStringLiteral("Can not open file."));
        QCOMPARE(e2.sqlError(), QSqlError());

        Firfuorida::Error e3 = std::move(e2);
        QCOMPARE(e3.type(), Firfuorida::Error::FileSystemError);
        QCOMPARE(e3.text(), QStringLiteral("Can not open file."));
        QCOMPARE(e3.sqlError(), QSqlError());
    }
}

QTEST_MAIN(TestErrorObject)

#include "testerrorobject.moc"
