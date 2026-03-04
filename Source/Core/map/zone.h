#pragma once

#include "zone_base.h"


// 子类注册函数
typedef void (*RegisterModZonesFunc)(ZoneFactory* factory);

// 默认园区
class DefaultZone : public Zone {
public:
    DefaultZone();

    static std::string GetId();
    virtual std::string GetType() const;
    virtual std::string GetName() const;

    static std::function<int(Plot*)> ZoneGenerator;
    virtual void SetAcreage(Plot* plot);
    virtual void AddBuilding(Plot* plot, BuildingFactory* buildingFactory);

private:
    static int count;

    int name;
};

