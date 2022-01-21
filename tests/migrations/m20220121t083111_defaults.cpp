#include "m20220121t083111_defaults.h"

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
    t->integer(QStringLiteral("intCol"))->defaultValue(123);
    t->date(QStringLiteral("dateCol"))->defaultValue(QStringLiteral("CURRENT_DATE"));
    t->text(QStringLiteral("textCol"))->defaultValue(QStringLiteral("dummer schiss"));
}

void M20220121T083111_Defaults::down()
{
    drop(QStringLiteral("defaultsTests"));
}

#include "moc_m20220121t083111_defaults.cpp"

