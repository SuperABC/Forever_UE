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
		std::function<bool(int, int, std::string, float)> setElement,
		std::function<std::string(int, int)> getTerrain, std::function<float(int, int)> getHeight) const = 0;
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
	std::unordered_map<
		std::string,
		std::pair<std::function<TerrainMod* ()>, std::function<void(TerrainMod*)>>
	> registries;
	std::unordered_map<std::string, bool> configs;
};

