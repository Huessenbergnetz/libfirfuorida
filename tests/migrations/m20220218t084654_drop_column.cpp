#include "m20220218t084654_drop_column.h"

M20220218T084654_Drop_column::M20220218T084654_Drop_column(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220218T084654_Drop_column::~M20220218T084654_Drop_column()
{

}

void M20220218T084654_Drop_column::up()
{
    auto t = table(QStringLiteral("tiny"));
    t->integer(QStringLiteral("colToDrop"));
}

void M20220218T084654_Drop_column::down()
{
    auto t = table(QStringLiteral("tiny"));
    t->dropColumn(QStringLiteral("colToDrop"));
}

#include "moc_m20220218t084654_drop_column.cpp"

