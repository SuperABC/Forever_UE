#pragma once

#include "traffic/station_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModStationsFunc)(StationFactory* factory);

// 子类注册收尾函数
typedef void (*FinishModStationsFunc)(StationFactory* factory);

// 站点实体
class Station {
public:
	/*
	* 禁止默认构造
	*/
	Station() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 站点工厂
	* @station: 站点类型标识
	*/
	Station(StationFactory* factory, const std::string& station);

	/*
	* 析构站点
	*/
	~Station();

	/*
	* 站点动态类型标识
	* @return: 动态类型标识
	*/
	std::string GetType() const;

	/*
	* 站点实例唯一名称
	* @return: 实例唯一名称
	*/
	std::string GetName() const;

	/*
	* 获取所在建筑名称
	* @return: 所在建筑名称
	*/
	std::string GetBuilding() const;

	/*
	* 设置所在建筑名称
	* @building: 建筑名称
	*/
	void SetBuilding(std::string building);

	/*
	* 布局站点
	* @block: 所在地块
	*/
	void LayoutStation(const Lot* block);

	/*
	* 获取建筑类型
	* @return: 建筑类型
	*/
	std::string GetBuildingType() const;

	/*
	* 获取建筑面积
	* @return: 建筑面积
	*/
	float GetBuildingAcreage() const;

	/*
	* 放置接口节点
	* @building: 所在建筑的几何形状
	* @pivots: 建筑的关键点集合
	*/
	void PlaceInterface(const Quad* building, const std::vector<Node*>& pivots);

	/*
	* 获取接口向量
	* @return: 接口向量
	*/
	std::vector<std::pair<std::string, std::pair<Node*, Node*>>> GetInterfaces() const;

	/*
	* 获取站点节点
	* @return: 站点节点
	*/
	std::vector<Node*> GetNodes() const;

private:
	// 模组对象
	OBJECT_HOLDER StationMod* mod;

	// 工厂
	StationFactory* factory;

	// 站点类型
	std::string type;

	// 站点名称
	std::string name;

	// 所在建筑名称
	std::string building;

	// 站点节点
	std::vector<Node*> nodes;
};

// 空站点
class EmptyStation : public StationMod {
public:
	/*
	* 构造空站点
	*/
	EmptyStation();

	/*
	* 析构空站点
	*/
	virtual ~EmptyStation();

	/*
	* 空站点静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 空站点动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 空站点实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 分配站点
	* @blocks: 地图全部地块
	* @return: 每个地块分配的站点数量
	*/
	static std::function<std::vector<int>& (const std::vector<Lot*>& blocks)> StationAssigner;

	/*
	* 布局站点
	* @block: 所在地块
	*/
	virtual void LayoutStation(const Lot* block);

	/*
	* 放置接口节点
	* @building: 所在建筑的几何形状
	* @pivots: 建筑的关键点集合
	*/
	virtual void PlaceInterface(const Quad* building, const std::vector<Node*>& pivots);

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;
};
