#pragma once

#include "map/building_mod.h"
#include "traffic/station_mod.h"


// 机场站点
class AirportStation : public StationMod {
public:
	/*
	* 构造机场站点
	*/
	AirportStation();

	/*
	* 析构机场站点
	*/
	virtual ~AirportStation();

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

	// 站点分配器
	static std::function<std::vector<int>& (const std::vector<Lot*>& blocks)> StationAssigner;

	/*
	* Override
	* 布局机场站点
	* @block: 所在地块
	*/
	virtual void LayoutStation(const Lot* block);

	/*
	* Override
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

// 机场建筑
class AirportBuilding : public BuildingMod {
public:
	/*
	* 构造机场建筑
	*/
	AirportBuilding();

	/*
	* 析构机场建筑
	*/
	virtual ~AirportBuilding();

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
	* 获取机场供电容量配置
	*/
	static std::vector<float> GetPowers();

	// 建筑分配器
	static std::function<int(const Lot*, int, int)> BuildingAssigner;

	/*
	* Override
	* 随机生成建筑面积
	*/
	virtual float RandomAcreage() override;

	/*
	* Override
	* 布局建筑内部
	* @quad: 建筑所占据的矩形区域
	*/
	virtual void LayoutBuilding(const Quad* quad) override;

	/*
	* Override
	* 放置建筑结构
	*/
	virtual void PlaceConstruction() override;

	/*
	* Override
	* 放置寻址锚点
	* @building: 建筑所占据的矩形区域
	*/
	virtual void PlacePivots(Quad* building) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;

	// 建筑朝向
	int direction;
};
