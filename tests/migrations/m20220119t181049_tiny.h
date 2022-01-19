#ifndef M20220119t181049_TINY_H
#define M20220119t181049_TINY_H

#include "../../Firfuorida/migration.h"

class M20220119t181049_Tiny : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M20220119t181049_Tiny)
public:
    explicit M20220119t181049_Tiny(Firfuorida::Migrator *parent);
    ~M20220119t181049_Tiny() override;

    void up() override;
    void down() override;
};

#endif // M20220119t181049_TINY_H

