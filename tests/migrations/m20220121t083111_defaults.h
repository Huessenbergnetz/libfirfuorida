#ifndef M20220121T083111_DEFAULTS_H
#define M20220121T083111_DEFAULTS_H

#include "../../Firfuorida/migration.h"

class M20220121T083111_Defaults : public Firfuorida::Migration
{
    Q_OBJECT
    Q_DISABLE_COPY(M20220121T083111_Defaults)
public:
    explicit M20220121T083111_Defaults(Firfuorida::Migrator *parent);
    ~M20220121T083111_Defaults() override;

    void up() override;
    void down() override;
};

#endif // M20220121T083111_DEFAULTS_H

