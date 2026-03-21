#pragma once

#include "zone_mod.h"


// 住宅园区
class ResidentialZone : public ZoneMod {
public:
    ResidentialZone();
    virtual ~ResidentialZone();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    static std::function<int(Lot*)> ZoneAssigner;

    virtual void AssignZone(Lot* plot);

private:
    static int count;

    int id;
};
