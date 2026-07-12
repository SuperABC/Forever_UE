#pragma once

#include "traffic/route_mod.h"


// 飞机线路
class AirRoute : public RouteMod {
public:
	/*
	* 构造飞机线路
	*/
	AirRoute();

	/*
	* 析构飞机线路
	*/
	virtual ~AirRoute();

	/*
	* Override
	* 统一类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 获取实例类型
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 获取实例名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 布局飞机航线
	* @interfaces: 按交通类型分组的站点接口集合
	* @sizeX, sizeY: 地图尺寸
	* @nodeStaticCount: 节点静态计数起点
	*/
	virtual void LayoutRoute(
		const std::unordered_map<std::string, std::vector<std::pair<std::vector<Node*>, std::vector<std::pair<Node*, Node*>>>>>& interfaces,
		int sizeX, int sizeY, int nodeStaticCount);

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;
};
