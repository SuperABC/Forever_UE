#pragma once

#include "../common/quad.h"
#include "../common/plot.h"

#include "building_base.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>


class Quad;
class Plot;
class Building;
class ZoneFactory;
class BuildingFactory;

class Zone : public Quad {
public:
    Zone() = default;
    virtual ~Zone();

	// 子类实现方法

    // 动态返回园区静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 在地图中生成园区
    static std::function<void(ZoneFactory*, const std::vector<Plot*>&)> ZoneGenerator;

	// 父类实现方法

    // 关联地块
    void SetParent(Plot* plot);
    Plot* GetParent() const;

    // 获取/设置房东
    int GetOwner() const;
    void SetOwner(int id);
	bool GetStateOwned() const;
	void SetStateOwned(bool state);

    // 获取/添加园区内建筑
    Building* GetBuilding(std::string name);
    std::unordered_map<std::string, Building*>& GetBuildings();
    void AddBuildings(BuildingFactory* factory, std::vector<std::pair<std::string, float>> list);

    // 自动分布建筑
    void ArrangeBuildings();

    // 动态位置
    std::pair<float, float> GetPosition() const;

protected:
    Plot* parentPlot;

	bool stateOwned = false;
    int ownerId = -1;

    std::unordered_map<std::string, Building*> buildings;
};

class ZoneFactory {
public:
    using GeneratorFunc = std::function<void(
        ZoneFactory *, BuildingFactory*, const std::vector<Plot*>&)>;

    void RegisterZone(const std::string& id,
        std::function<Zone* ()> creator, GeneratorFunc generator = nullptr);
    Zone* CreateZone(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);
    void GenerateAll(const std::vector<Plot*>& plots, BuildingFactory* factory);

private:
    std::unordered_map<std::string, std::function<Zone* ()>> registries;
    std::unordered_map<std::string, bool> configs;
    std::unordered_map<std::string, GeneratorFunc> generators;
};
