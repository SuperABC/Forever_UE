#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


// 地形Mod基类
class TerrainMod {
public:
	/*
	* 构造地形
	*/
	TerrainMod();

	/*
	* 析构地形
	*/
	virtual ~TerrainMod();

	/*
	* Override
	* 地形静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 地形动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 地形实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 地形构建优先级
	*/
	COSTOM_INIT virtual float GetPriority() const = 0;

	/*
	* Override
	* 设置材质纹理
	*/
	COSTOM_INIT virtual void SetupTexture() = 0;

	/*
	* Override
	* 构建地形
	* @width, height: 地图element分辨率
	* @getTerrain, setTerrain: 获取/设置地图element处地形类型
	* @getHeight, setHeight: 获取/设置地图element处地形高度
	*/
	COSTOM_INIT virtual void DistributeTerrain(int width, int height,
		const std::function<std::string(int, int)>& getTerrain,
		const std::function<bool(int, int, std::string)>& setTerrain,
		const std::function<float(int, int)>& getHeight,
		const std::function<bool(int, int, float)>& setHeight) const = 0;

	/*
	* Tool
	* 地形滤波
	* @x, y: 滤波中心位置
	* @width, height: 地图element分辨率
	* @get, set: 获取/设置地图element处地形类型
	* @side: 滤波核半边长
	* @threshold: 滤波判定阈值
	*/
	void ShapeFilter(int x, int y, int width, int height,
		std::function<std::string(int, int)> get, std::function<bool(int, int, std::string)> set,
		int side = 1, float threshold = 0.5f) const;

	// 漫反射纹理路径
	std::string diffusePath;

	// 是否添加水面以及水面高度
	std::pair<bool, float> waterHeight;
};

class TerrainFactory {
public:
	/*
	* 注册地形
	* @id: 地形类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterTerrain(const std::string& id,
		std::function<TerrainMod* ()> creator, std::function<void(TerrainMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建地形
	* @id: 地形类型
	*/
	TerrainMod* CreateTerrain(const std::string& id) const;

	/*
	* 检查地形是否已注册
	* @id: 地形类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置地形启用配置
	* @id: 地形类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& id, bool config);

	/*
	* 获取所有已启用的地形静态类型标识
	*/
	std::vector<std::string> GetTerrains() const;

	/*
	* 析构地形
	* @terrainMod: 待析构的地形
	*/
	void DestroyTerrain(TerrainMod* terrainMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<TerrainMod* ()>, std::function<void(TerrainMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

