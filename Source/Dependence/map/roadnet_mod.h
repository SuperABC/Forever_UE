#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "geometry.h"

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>


// 路网Mod基类
class RoadnetMod {
public:
	/*
	* 构造路网
	*/
	RoadnetMod();

	/*
	* 析构路网
	*/
	virtual ~RoadnetMod();

	/*
	* Override
	* 路网静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 路网动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 路网实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 构建路网
	* @width, height: 地图分辨率
	* @getTerrain: 获取地图element处地形类型
	* @getHeight: 获取地图element处地形高度
	* @getWater: 获取地图element处水域信息（是否水域、水面高度）
	* @nodeStaticCount: 跨dll传Node::count（不要在mod里留下未传回的Node实例）
	*/
	COSTOM_INIT virtual void DistributeRoadnet(int width, int height,
		const std::function<std::string(int, int)>& getTerrain,
		const std::function<float(int, int)>& getHeight,
		const std::function<std::pair<bool, float>(int, int)>& getWater, int nodeStaticCount) = 0;

	/*
	* Tool
	* 根据连接线在t1、t2两点的位置生成一个开洞信息（隧道入口或出口的其中一个洞口），加入本模组的开洞列表；
	* 洞口沿道路方向的长度由t1、t2两点间距离决定，朝向为t1指向t2的方向
	* @connection: 隧道所在连接线
	* @t1, t2: 洞口沿连接线的起止参数位置
	* @width: 洞口垂直于道路方向的宽度
	*/
	void AddHatch(Connection* connection, float t1, float t2, float width);

	// 外部连接
	std::vector<Node> externs;

	// 路口
	std::vector<Intersection> intersections;

	// 道路
	std::vector<Road> roads;

	// 地块数据：Lot + 相邻道路(key为FACE_DIRECTION) + 角路口(key为CORNER_DIRECTION)
	std::vector<std::pair<Lot, std::pair<std::unordered_map<int, Road>, std::unordered_map<int, Intersection>>>> lots;

	// 开洞列表（隧道入口出口等）
	std::vector<std::pair<Quad, float>> hatches;
};

class RoadnetFactory {
public:
	/*
	* 注册路网
	* @id: 路网类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterRoadnet(const std::string& id,
		std::function<RoadnetMod* ()> creator, std::function<void(RoadnetMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建路网
	* @id: 路网类型
	*/
	RoadnetMod* CreateRoadnet(const std::string& id) const;

	/*
	* 检查路网是否已注册
	* @id: 路网类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置路网启用配置
	* @name: 路网类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取当前启用的路网静态类型标识
	*/
	std::string GetRoadnet() const;

	/*
	* 析构路网
	* @roadnetMod: 待析构的路网
	*/
	void DestroyRoadnet(RoadnetMod* roadnetMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<RoadnetMod* ()>, std::function<void(RoadnetMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

