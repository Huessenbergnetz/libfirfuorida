#include "m20220129t115731_foreignkey2.h"

M20220129T115731_Foreignkey2::M20220129T115731_Foreignkey2(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220129T115731_Foreignkey2::~M20220129T115731_Foreignkey2()
{

}

void M20220129T115731_Foreignkey2::up()
{
    auto t = create(QStringLiteral("table2"));
    t->increments();
    t->integer(QStringLiteral("table1_id"))->unSigned();
    t->text(QStringLiteral("address"));
    t->foreignKey(QStringLiteral("table1_id"), QStringLiteral("table1"), QStringLiteral("id"), QStringLiteral("id_table1_id"))->onDelete(QStringLiteral("CASCADE"))->onUpdate(QStringLiteral("CASCADE"));
}

void M20220129T115731_Foreignkey2::down()
{
    drop(QStringLiteral("table2"));
}

#include "moc_m20220129t115731_foreignkey2.cpp"

