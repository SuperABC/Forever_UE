#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>


class Terrain {
public:
    Terrain();
    virtual ~Terrain();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 动态返回生成优先级，高优先级会被低优先级覆盖
    virtual float GetPriority() const = 0;

    // 在地图上生成地形
    virtual void DistributeTerrain(int width, int height,
        std::function<bool(int, int, const std::string&, float)> setElement,
        std::function<std::string(int, int)> getTerrain,
        std::function<float(int, int)> getHeight) const = 0;

    // 父类实现方法

    // 地形填充，若ovewrite为true，则全图填充，否则只填充平原
    int FloodTerrain(
        int x, int y, int num, bool overwrite, int width, int height,
        std::function<bool(int, int, const std::string&, float)> set,
        std::function<std::string(int, int)> get) const;

protected:
    // 检查地形填充处是否为当前边界
    bool CheckBoundary(
        int x, int y, bool overwrite, int width, int height,
        std::function<bool(int, int, const std::string&, float)> set,
        std::function<std::string(int, int)> get) const;

    // 更新地形填充边界
    void UpdateBoundary(
        int x, int y, std::vector<std::pair<int, int>>& q, bool overwrite, int width, int height,
        std::function<bool(int, int, const std::string&, float)> set,
        std::function<std::string(int, int)> get) const;

    // 地形滤波
    void ShapeFilter(int x, int y, int width, int height,
        std::function<bool(int, int, const std::string&, float)> set,
        std::function<std::string(int, int)> get, int side = 1, float threshold = 0.5f) const;

private:
    std::vector<int> dx;
    std::vector<int> dy;
};

class TerrainFactory {
public:
    // 构造地形（包含new操作）
    void RegisterTerrain(const std::string& id,
        std::function<Terrain* ()> creator, std::function<void(Terrain*)> deleter);

    // 创建地形
    Terrain* CreateTerrain(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 获取所有启用地形
    std::vector<Terrain*> GetTerrains() const;

    // 析构地形（包含delete操作）
    void DestroyTerrains(const std::vector<Terrain*>& terrains) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Terrain* ()>, std::function<void(Terrain*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};