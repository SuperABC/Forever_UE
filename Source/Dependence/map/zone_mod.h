#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/geometry.h"

#include <string>
#include <functional>


class ZoneMod : public Quad {
public:
    // 无构造
    ZoneMod();

    // 无析构
    virtual ~ZoneMod();

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 指定园区数量
    static std::function<int(Lot*)> ZoneAssigner;

    // 设计园区
    virtual void AssignZone(Lot* lot) = 0;

    // 内部建筑
    std::vector<std::pair<std::string, float>> buildings;
};

class ZoneFactory {
public:
    // 注册园区
    void RegisterZone(const std::string& id, std::function<int(Lot*)> assigner,
        std::function<ZoneMod* ()> creator, std::function<void(ZoneMod*)> deleter);

    // 清空注册
    void RemoveAll();

    // 创建园区
    ZoneMod* CreateZone(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 获取所有启用园区
    std::vector<std::string> GetTypes() const;

    // 在地块内生成一类园区
    std::vector<ZoneMod*> CreateZones(const std::string& type, Lot* lot) const;

    // 析构园区
    void DestroyZone(ZoneMod* zone) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<ZoneMod* ()>, std::function<void(ZoneMod*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
    std::unordered_map<std::string, std::function<int(Lot*)>> assigners;
};

