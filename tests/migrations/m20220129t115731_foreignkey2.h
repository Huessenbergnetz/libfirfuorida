#ifndef M20220129T115731_FOREIGNKEY2_H
#define M20220129T115731_FOREIGNKEY2_H

#include <Firfuorida/migration.h>

class M20220129T115731_Foreignkey2 : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M20220129T115731_Foreignkey2)
public:
    explicit M20220129T115731_Foreignkey2(Firfuorida::Migrator *parent);
    ~M20220129T115731_Foreignkey2() override;

    void up() override;
    void down() override;
};

#endif // M20220129T115731_FOREIGNKEY2_H

