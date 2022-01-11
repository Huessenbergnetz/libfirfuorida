#include "example.h"

M20190121T174100_Example::M20190121T174100_Example(Firfuorida::Migrator *parent) :
    Firefuorida::Migration(parent)
{
}

M20190121T174100_Example::~M20190121T174100_Example()
{
}

void M20190121T174100_Example::up()
{
    // create a new table named "ExampleTable" if it not already exists
    auto t = createTableIfNotExists(QStringLiteral("ExampleTable"));
    // create a new unsigned tiny integer column on that table with the
    // default name "id" and auto incrementation enabled
    t->tinyIncrements();
    // create a new signed tiny integer column on that table with the
    // name "tinyIntCol" and UNIQUE KEY index.
    t->tinyInteger(QStringLiteral("tinyIntCol"))->unique();
    // create a new nullable tiny blob column on that table with the name
    // "tinyBlobCol"
    t->tinyBlob(QStringLiteral("tinyBlobCol"))->nullable();
    // create a new tiny text column on that table with the name "tinyTextCol"
    // and the default value "foobar"
    t->tinyText(QStringLiteral("tinyTextCol"))->defaultValue(QStringLiteral("foobar"));
}

void M20190121T174100_Example::down()
{
    // drop the table named "ExampleTable" if it exists
    dropIfExists(QStringLiteral("ExampleTable"));
}
