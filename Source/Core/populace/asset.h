#pragma once

#include "asset_base.h"
#include "zone_base.h"


// 子类注册函数
typedef void (*RegisterModAssetsFunc)(AssetFactory* factory);

// 园区资产子类
class ZoneAsset : public Asset {
public:
    ZoneAsset(Zone* zone);
    ~ZoneAsset();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    Zone* GetZone();
    void SetZone(Zone* zone);

private:
    Zone* zone;
};

// 建筑资产子类
class BuildingAsset : public Asset {
public:
    BuildingAsset(Building* building);
    ~BuildingAsset();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    Building* GetBuilding();
    void SetBuilding(Building* building);

private:
    Building* building;
};

// 房间资产子类
class RoomAsset : public Asset {
public:
    RoomAsset(Room* room);
    ~RoomAsset();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    Room* GetRoom();
    void SetRoom(Room* room);

private:
    Room* room;
};
