#include "m20220121t083111_defaults.h"
#include <QDateTime>

M20220121T083111_Defaults::M20220121T083111_Defaults(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220121T083111_Defaults::~M20220121T083111_Defaults()
{

}

void M20220121T083111_Defaults::up()
{
    auto t = create(QStringLiteral("defaultTests"));
    t->increments();
    t->integer(QStringLiteral("noDefValCol"));
    t->integer(QStringLiteral("intCol"))->defaultValue(123);
    if (dbType() == Firfuorida::Migrator::SQLite) {
        t->date(QStringLiteral("dateCol"))->defaultValue(QStringLiteral("CURRENT_DATE"));
    } else {
        t->dateTime(QStringLiteral("dateCol"))->defaultValue(QStringLiteral("CURRENT_TIMESTAMP"));
    }
    t->text(QStringLiteral("textCol"))->defaultValue(QStringLiteral("dummer schiss"));
    if (dbType() == Firfuorida::Migrator::SQLite) {
        t->floatCol(QStringLiteral("exprCol"), 20, 10)->defaultValue(QStringLiteral("(random() * random())"));
    } else {
        t->floatCol(QStringLiteral("exprCol"), 20, 10)->defaultValue(QStringLiteral("(RAND() * RAND())"));
    }
    t->floatCol(QStringLiteral("floatCol"), 5, 3)->defaultValue(12.34);
    t->blob(QStringLiteral("blobCol"))->defaultValue(QStringLiteral("Hallo Welt!/"));
    t->dateTime(QStringLiteral("dateTimeCol1"))->defaultValue(QDateTime::currentDateTime());
    t->dateTime(QStringLiteral("dateTimeCol2"))->defaultValue(QStringLiteral("2022-01-21 13:00:02.460"));
}

void M20220121T083111_Defaults::down()
{
    drop(QStringLiteral("defaultsTests"));
}

#include "moc_m20220121t083111_defaults.cpp"

