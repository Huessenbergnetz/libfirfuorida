#include "m20220119t181249_small.h"

M20220119T181249_Small::M20220119T181249_Small(Firfuorida::Migrator *parent) :
    Firfuorida::Migration(parent)
{

}

M20220119T181249_Small::~M20220119T181249_Small()
{

}

void M20220119T181249_Small::up()
{
    auto t = create(QStringLiteral("small"));
    t->smallIncrements();
    t->smallInteger(QStringLiteral("smallIntCol"));
    t->blob(QStringLiteral("blobCol"));
    t->text(QStringLiteral("textCol"));
}

void M20220119T181249_Small::down()
{
    drop(QStringLiteral("small"));
}

#include "moc_m20220119t181249_small.cpp"

