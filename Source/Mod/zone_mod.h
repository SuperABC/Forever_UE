#pragma once

#include "zone_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModZone : public Zone {
public:
    ModZone() {
        name = count++;
    }

    static std::string GetId() { return "mod"; }
    virtual std::string GetType() const override { return "mod"; }
    virtual std::string GetName() const override { return "模组园区" + std::to_string(name); }

    static std::function<int(Plot*)> ZoneGenerator;
    virtual void SetZone(Plot* plot, BuildingFactory* buildingFactory) {
        Quad::SetAcreage(10000.f);
        AddBuildings(buildingFactory, { {"mod", 1.f}, {"mod", 1.f} });
    }

private:
    static int count;

    int name;
};

int ModZone::count = 0;

std::function<int(Plot*)> ModZone::ZoneGenerator = [](Plot* plot) { return 1; };