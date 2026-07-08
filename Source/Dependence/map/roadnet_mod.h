#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "geometry.h"

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>


class RoadnetMod {
public:
	/*
	* 无构造
	*/
	RoadnetMod();

	/*
	* 无析构
	*/
	virtual ~RoadnetMod();

	/*
	* Override
	* 路网静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 路网动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 路网实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 构建路网
	* @width, height: 地图分辨率
	* @get: 获取地图element处地形类型
	* @nodeStaticCount: 跨dll传Node::count（不要在mod里留下未传回的Node实例）
	*/
	COSTOM_INIT virtual void DistributeRoadnet(int width, int height,
		const std::function<std::string(int, int)>& get, int nodeStaticCount) = 0;

	// 外部连接
	std::vector<Node> externs;

	// 路口
	std::vector<Intersection> intersections;

	// 道路
	std::vector<Road> roads;

	// 地块数据：Lot + 相邻道路(key为FACE_DIRECTION) + 角路口(key为CORNER_DIRECTION)
	std::vector<std::pair<Lot, std::pair<std::unordered_map<int, Road>, std::unordered_map<int, Intersection>>>> lots;
};

class RoadnetFactory {
public:
	/*
	* 注册路网及其创建与销毁方式
	* @id: 路网静态类型标识
	* @creator, deleter: 创建/销毁路网实例的方法
	*/
	void RegisterRoadnet(const std::string& id,
		std::function<RoadnetMod* ()> creator, std::function<void(RoadnetMod*)> deleter);

	/*
	* 清空所有启用的路网配置
	*/
	void RemoveAll();

	/*
	* 按静态类型标识创建路网实例
	* @id: 路网静态类型标识
	*/
	RoadnetMod* CreateRoadnet(const std::string& id) const;

	/*
	* 检查路网是否已注册
	* @id: 路网静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置路网的启用配置
	* @name: 路网静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取当前启用的路网静态类型标识
	*/
	std::string GetRoadnet() const;

	/*
	* 销毁路网实例
	* @roadnetMod: 待销毁的路网实例
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

