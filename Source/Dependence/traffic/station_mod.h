#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../map/geometry.h"

#include <string>
#include <functional>


class StationMod {
public:
	/*
	* 构造站点模组
	*/
	StationMod();

	/*
	* 析构站点模组
	*/
	virtual ~StationMod();

	/*
	* Override
	* 统一类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 获取实例类型
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 获取实例名称
	*/
	virtual const char* GetName() = 0;

	// 站点分配器
	COSTOM_INIT static std::function<std::vector<int>&(const std::vector<Lot*>& blocks)> StationAssigner;

	/*
	* Override
	* 布局站点
	* @block: 所在地块
	*/
	COSTOM_INIT virtual void LayoutStation(const Lot* block) = 0;

	/*
	* Override
	* 放置接口节点
	* @building: 所在建筑的几何形状
	* @pivots: 建筑的关键点集合
	*/
	COSTOM_INIT virtual void PlaceInterface(const Quad* building, const std::vector<Node*>& pivots) = 0;

	// 建筑类型
	std::string buildingType;

	// 建筑面积
	float buildingAcreage;

	// 站点节点
	std::vector<Node*> nodes;

	// 接口向量（交通类型 → 起点与方向节点对）
	std::vector<std::pair<std::string, std::pair<Node*, Node*>>> interfaces;
};

class StationFactory {
public:
	/*
	* 注册站点类型
	* @id: 类型标识
	* @assigner: 站点分配器
	* @creator, deleter: 创建与析构函数
	*/
	void RegisterStation(const std::string& id,
		std::function<std::vector<int>& (const std::vector<Lot*>& blocks)> assigner,
		std::function<StationMod* ()> creator, std::function<void(StationMod*)> deleter);

	/*
	* 清空注册表
	*/
	void RemoveAll();

	/*
	* 按类型创建站点对象
	* @id: 类型标识
	*/
	StationMod* CreateStation(const std::string& id) const;

	/*
	* 检查类型是否已注册
	* @id: 类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 类型名称
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取所有已启用的站点类型
	*/
	std::vector<std::string> GetTypes() const;

	/*
	* 分配一类站点到地块
	* @type: 站点类型标识
	* @blocks: 候选地块集合
	*/
	std::vector<int> AssignStations(const std::string& type, const std::vector<Lot*>& blocks) const;

	/*
	* 析构站点对象
	* @stationMod: 待析构的站点对象
	*/
	void DestroyStation(StationMod* stationMod) const;

private:
	// 注册表（类型标识 → 创建/析构函数对）
	std::unordered_map<
		std::string,
		std::pair<std::function<StationMod* ()>, std::function<void(StationMod*)>>
	> registries;

	// 启用配置（类型标识 → 是否启用）
	std::unordered_map<std::string, bool> configs;

	// 站点分配器（类型标识 → 分配器函数）
	std::unordered_map<std::string,
		std::function<std::vector<int>& (const std::vector<Lot*>& blocks)>> assigners;
};
