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
	* @get: 获取地图element处地形类型
	*/
	virtual void DistributeRoadnet(int width, int height,
		const std::function<std::string(int, int)>& get, int nodeStaticCount) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};
