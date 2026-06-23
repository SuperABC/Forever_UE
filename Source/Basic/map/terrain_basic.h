#pragma once

#include "map/terrain_mod.h"


// 海洋地形
class OceanTerrain : public TerrainMod {
public:
	/*
	* 构造海洋地形
	*/
	OceanTerrain();

	/*
	* 析构海洋地形
	*/
	virtual ~OceanTerrain();

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

// 山区地形
class MountainTerrain : public TerrainMod {
public:
	/*
	* 构造山区地形
	*/
	MountainTerrain();

	/*
	* 析构山区地形
	*/
	virtual ~MountainTerrain();

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
