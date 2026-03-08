#pragma once

#include "../common/quad.h"
#include "../common/plot.h"
#include "../common/utility.h"

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
    Zone();
    virtual ~Zone();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 单一园区设置
    virtual void SetZone(Plot* plot, BuildingFactory* buildingFactory) = 0;

    // 地块园区数量函数
    static std::function<int(Plot*)> ZoneGenerator;

    // 父类实现方法

    // 获取所在地块
    Plot* GetParent() const;

    // 设置所在地块
    void SetParent(Plot* plot);

    // 获取私人房东ID
    int GetOwner() const;

    // 设置私人房东ID
    void SetOwner(int owner);

    // 获取是否由政府拥有
    bool GetStated() const;

    // 设置是否由政府拥有
    void SetStated(bool state);

    // 获取一栋建筑
    Building* GetBuilding(const std::string& name);

    // 获取园区内所有建筑
    std::unordered_map<std::string, Building*>& GetBuildings();

    // 添加园区内建筑
    void AddBuildings(BuildingFactory* factory, const std::vector<std::pair<std::string, float>>& list);

    // 自动分布建筑
    void ArrangeBuildings();

    // 获取世界位置
    std::pair<float, float> GetPosition() const;

    // 获取地址
    std::string GetAddress() const;

private:
    // 所在地块
    Plot* parentPlot;

    // 是否由政府拥有
    bool stated;

    // 私人房东ID
    int owner;

    // 园区内建筑
    std::unordered_map<std::string, Building*> buildings;
};

class ZoneFactory {
public:
    using GeneratorFunc = std::function<int(Plot*)>;

    // 注册园区
    void RegisterZone(const std::string& id, GeneratorFunc generator,
        std::function<Zone* ()> creator, std::function<void(Zone*)> deleter);

    // 创建园区（包含new操作）
    Zone* CreateZone(const std::string& id);

    // 检查是否注册
    bool CheckRegistered(const std::string& id);

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 获取所有启用园区
    std::vector<std::string> GetTypes();

    // 在地块内生成一类园区
    std::vector<Zone*> CreateZones(const std::string& type, Plot* plot);

    // 析构园区（包含delete操作）
    void DestroyZone(Zone* zone);

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Zone* ()>, std::function<void(Zone*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
    std::unordered_map<std::string, GeneratorFunc> generators;
};