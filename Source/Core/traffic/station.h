#pragma once

#include "traffic/station_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModStationsFunc)(StationFactory* factory);

// 站点实体
class Station {
public:
	// 禁止默认构造
	Station() = delete;

	// 通过类型从工厂构造
	Station(StationFactory* factory, const std::string& station);

	// 析构站点
	~Station();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取所在建筑
	std::string GetBuilding() const;

	// 设置所在建筑
	void SetBuilding(std::string building);

	// 布局站点
	void LayoutStation(const Lot* block);

	// 获取建筑类型
	std::string GetBuildingType() const;

	// 获取建筑面积
	float GetBuildingAcreage() const;

	// 放置接口
	void PlaceInterface(const Quad* building, const std::vector<Node*>& pivots);

	// 获取接口
	std::vector<std::pair<std::string, std::pair<Node*, Node*>>> GetInterfaces() const;

	// 获取站点节点
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
	EmptyStation();
	virtual ~EmptyStation();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::function<std::vector<int>& (const std::vector<Lot*>& blocks)> StationAssigner;

	virtual void LayoutStation(const Lot* block);

	virtual void PlaceInterface(const Quad* building, const std::vector<Node*>& pivots);

private:
	static int count;

	int id;
	std::string name;
};

