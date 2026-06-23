#pragma once

#include "map/terrain_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModTerrainsFunc)(TerrainFactory* factory);

// 地形实体
class Terrain {
public:
	/*
	* 禁止默认构造
	*/
	Terrain() = delete;

	/*
	* 通过地形静态类型标识构造地形
	* @factory: 地形工厂
	* @terrain: 地形静态类型标识
	*/
	Terrain(TerrainFactory* factory, const std::string& terrain);

	/*
	* 析构地形
	*/
	~Terrain();

	/*
	* 获取地形动态类型标识
	*/
	std::string GetType() const;

	/*
	* 获取地形实例名称
	*/
	std::string GetName() const;

	/*
	* 获取构建优先级
	*/
	float GetPriority() const;

	/*
	* 设置地形材质
	*/
	void SetupTexture() const;

	/*
	* 获取地形材质
	*/
	std::string GetTexture() const;

	/*
	* 获取水面信息
	*/
	std::pair<bool, float> GetWater() const;

	/*
	* 构建地形
	* @width, height: 地图分辨率
	* @getTerrain, setTerrain: 获取/设置地图处地形类型
	* @getHeight, setHeight: 获取/设置地图处地形高度
	*/
	void DistributeTerrain(int width, int height,
		const std::function<std::string(int, int)>& getTerrain,
		const std::function<bool(int, int, std::string)>& setTerrain,
		const std::function<float(int, int)>& getHeight,
		const std::function<bool(int, int, float)>& setHeight) const;

private:
	// 模组对象
	OBJECT_HOLDER TerrainMod* mod;

	// 工厂
	TerrainFactory* factory;

	// 地形类型
	std::string type;

	// 地形名称
	std::string name;
};

// 空地形
class EmptyTerrain : public TerrainMod {
public:
	/*
	* 构造空地形
	*/
	EmptyTerrain();

	/*
	* 析构空地形
	*/
	virtual ~EmptyTerrain();

	/*
	* Override
	* 地形静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 地形动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 地形实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 地形构建优先级
	*/
	virtual float GetPriority() const override;

	/*
	* Override
	* 设置材质纹理
	*/
	virtual void SetupTexture() override;

	/*
	* Override
	* 构建地形
	* @width, height: 地图分辨率
	* @getTerrain, setTerrain: 获取/设置地图处地形类型
	* @getHeight, setHeight: 获取/设置地图处地形高度
	*/
	virtual void DistributeTerrain(int width, int height,
		const std::function<std::string(int, int)>& getTerrain,
		const std::function<bool(int, int, std::string)>& setTerrain,
		const std::function<float(int, int)>& getHeight,
		const std::function<bool(int, int, float)>& setHeight) const override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

