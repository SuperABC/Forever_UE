#pragma once

#include "zone_mod.h"

#include "block.h"


// 子类注册函数
typedef void (*RegisterModZonesFunc)(ZoneFactory* factory);

// 空园区
class EmptyZone : public ZoneMod {
public:
    EmptyZone();
    virtual ~EmptyZone();

    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

    static std::function<int(Lot*)> ZoneAssigner;

    virtual void LayoutZone(Lot* block);

private:
    static int count;

    int id;
};

// 园区实体
class Block;
class Zone {
public:
    Zone() = delete;
    Zone(ZoneFactory* factory, std::string zone);
    ~Zone();

    // 获取类型
    std::string GetType() const;

    // 获取名称
    std::string GetName() const;

    // 设计园区
    void LayoutZone(Lot* block);

    // 获取所在地块
    Block* GetParent() const;

    // 设置所在地块
    void SetParent(Block* block);

    // 获取私人房东ID
    int GetOwner() const;

    // 设置私人房东ID
    void SetOwner(int owner);

    // 获取是否由政府拥有
    bool GetStated() const;

    // 设置是否由政府拥有
    void SetStated(bool state);

    // 获取一栋建筑
    //Building* GetBuilding(const char* name);

    // 获取园区内所有建筑
    //const std::unordered_map<std::string, Building*>& GetBuildings();

    // 添加园区内建筑
    //void AddBuildings(BuildingFactory* factory, const std::vector<std::pair<std::string, float>>& list);

    // 自动分布建筑
    //void ArrangeBuildings();

    // 获取园区中心世界位置
    void GetPosition(float& x, float& y) const;

    // 获取地址
    const char* GetAddress();

private:
    ZoneMod* mod;
    ZoneFactory* factory;

    std::string type;
    std::string name;

    Block* parentBlock;
    std::string fullAddress;
    bool stated;
    //Person* owner;
    //VARIABLE_HOLDER std::unordered_map<std::string, Building*> buildings;
};

