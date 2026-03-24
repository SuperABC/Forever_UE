#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class TerrainMod {
public:
	// 无构造
	TerrainMod();

	// 无析构
	virtual ~TerrainMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 构建优先级
	COMSTOM_INIT virtual float GetPriority() const = 0;

	// 构建地形
	COMSTOM_INIT virtual void DistributeTerrain(int width, int height,
		std::function<std::string(int, int)> getTerrain, 
		std::function<bool(int, int, std::string)> setTerrain,
        std::function<float(int, int)> getHeight, 
		std::function<bool(int, int, float)> setHeight) const = 0;

    // 地形填充，若ovewrite为true，则全图填充，否则只填充平原
    int FloodTerrain(
        int x, int y, int num, bool overwrite, int width, int height,
        std::function<std::string(int, int)> get, std::function<bool(int, int, std::string)> set) const;
        
    // 检查地形填充处是否为当前边界
    bool CheckBoundary(
        int x, int y, bool overwrite, int width, int height,
        std::function<std::string(int, int)> get, std::function<bool(int, int, std::string)> set) const;

    // 更新地形填充边界
    void UpdateBoundary(
        int x, int y, std::vector<std::pair<int, int>>& q, bool overwrite, int width, int height,
        std::function<std::string(int, int)> get, std::function<bool(int, int, std::string)> set) const;

    // 地形滤波
    void ShapeFilter(int x, int y, int width, int height,
        std::function<std::string(int, int)> get, std::function<bool(int, int, std::string)> set,
        int side = 1, float threshold = 0.5f) const;

    // 常量算子
    std::vector<int> dx, dy;
};

class TerrainFactory {
public:
	// 注册地形
	void RegisterTerrain(const std::string& id,
		std::function<TerrainMod* ()> creator, std::function<void(TerrainMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建地形
	TerrainMod* CreateTerrain(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取所有启用地形名
	std::vector<std::string> GetTerrains() const;

	// 析构地形
	void DestroyTerrain(TerrainMod* terrain) const;

private:
    // 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<TerrainMod* ()>, std::function<void(TerrainMod*)>>
	> registries;
    
    // 启用配置
	std::unordered_map<std::string, bool> configs;
};

