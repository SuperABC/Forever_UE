#pragma once

#include "zone_base.h"


// 子类注册函数
typedef void (*RegisterModZonesFunc)(ZoneFactory* factory);

// 默认园区
class DefaultZone : public Zone {
public:
    DefaultZone();
    ~DefaultZone() override;

    static std::string GetId();
    std::string GetType() const override;
    std::string GetName() const override;

    static std::function<int(Plot*)> ZoneGenerator;
    void SetZone(Plot* plot, BuildingFactory* buildingFactory) override;

private:
    static int count;

    int name;
};