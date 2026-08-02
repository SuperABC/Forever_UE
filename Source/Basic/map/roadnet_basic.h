#pragma once

#include "map/roadnet_mod.h"


// 井字路网
class JingRoadnet : public RoadnetMod {
public:
	/*
	* 构造井字路网
	*/
	JingRoadnet();

	/*
	* 析构井字路网
	*/
	virtual ~JingRoadnet();

	/*
	* Override
	* 路网静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 路网动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 路网实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 构建路网
	* @width, height: 地图分辨率
	* @getTerrain: 获取地图element处地形类型
	* @getHeight: 获取地图element处地形高度
	* @getWater: 获取地图element处水域信息（是否水域、水面高度）
	*/
	virtual void DistributeRoadnet(int width, int height,
		const std::function<std::string(int, int)>& getTerrain,
		const std::function<float(int, int)>& getHeight,
		const std::function<std::pair<bool, float>(int, int)>& getWater, int nodeStaticCount) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
