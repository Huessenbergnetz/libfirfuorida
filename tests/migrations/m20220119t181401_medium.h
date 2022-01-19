#ifndef M20220119T181401_MEDIUM_H
#define M20220119T181401_MEDIUM_H

#include "../../Firfuorida/migration.h"

class M20220119T181401_Medium : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M20220119T181401_Medium)
public:
    explicit M20220119T181401_Medium(Firfuorida::Migrator *parent);
    ~M20220119T181401_Medium() override;

    void up() override;
    void down() override;
};

#endif // M20220119T181401_MEDIUM_H

