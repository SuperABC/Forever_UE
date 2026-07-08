#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include "map/geometry.h"

#include <vector>
#include <unordered_map>


class Block : public Lot {
public:
	/*
	* 构造空街区
	*/
	Block();

	/*
	* 根据地块构造街区
	* @lot: 地块
	*/
	Block(Lot lot);

	/*
	* 析构园区与建筑
	*/
	virtual ~Block();

	/*
	* 获取街区四个方向（FACE_DIRECTION）相邻的道路，某方向无路则为null
	*/
	const std::vector<Road*>& GetRoads() const;

	/*
	* 设置街区四个方向相邻道路（size为4，按FACE_DIRECTION顺序，无路方向设null）
	* @roads: 四个方向的Road指针
	*/
	void SetRoads(const std::vector<Road*>& roads);

	/*
	* 获取街区四个角（CORNER_DIRECTION）对应的路口，某角无路口则为null
	*/
	const std::vector<Intersection*>& GetIntersections() const;

	/*
	* 设置街区四个角对应的路口（size为4，按CORNER_DIRECTION顺序，无路口则设null）
	* @intersections: 四个角的Intersection指针
	*/
	void SetIntersections(const std::vector<Intersection*>& intersections);

	/*
	* 获取街区内部划分（DivideSpace）产生的导航节点
	*/
	const std::vector<Node*>& GetNodes() const;

	/*
	* 获取街区自身的四角四边（非持有引用）
	*/
	const QuadBoundary& GetBoundary() const;

	/*
	* 追加街区内部划分产生的导航节点（街区持有，负责释放）
	* @newNodes: 新增节点
	*/
	void AddNodes(const std::vector<Node*>& newNodes);

	/*
	* 设置街区自身的四角四边
	* @boundary: 四角四边（非持有引用）
	*/
	void SetBoundary(const QuadBoundary& boundary);

	/*
	* 获取全部园区
	*/
	std::unordered_map<std::string, Zone*>& GetZones();

	/*
	* 获取全部建筑
	*/
	std::unordered_map<std::string, Building*>& GetBuildings();

	/*
	* 添加一个园区
	* @name, zone: 园区名称, 园区
	*/
	void AddZone(const std::string& name, Zone* zone);

	/*
	* 添加一个建筑
	* @name, building: 建筑名称, 建筑
	*/
	void AddBuilding(const std::string& name, Building* building);

	/*
	* 按名称获取园区
	* @name: 园区名称
	*/
	Zone* GetZone(const std::string& name) const;

	/*
	* 按名称获取建筑
	* @name: 建筑名称
	*/
	Building* GetBuilding(const std::string& name) const;

	/*
	* 按名称移除园区
	* @name: 园区名称
	*/
	void RemoveZone(const std::string& name);

	/*
	* 按名称移除建筑
	* @name: 建筑名称
	*/
	void RemoveBuilding(const std::string& name);

	/*
	* 设置街区地址
	* @road, id: 道路名称, 编号
	*/
	void SetAddress(const std::string& road, int id);

	/*
	* 获取街区地址
	*/
	std::string GetAddress() const;

protected:
	// 四方向相邻道路
	OBJECT_HOLDER std::vector<Road*> roads;

	// 四角对应路口
	OBJECT_HOLDER std::vector<Intersection*> intersections;

	// 内部划分产生的导航节点
	OBJECT_HOLDER std::vector<Node*> nodes;

	// 自身的四角四边（非持有引用）
	QuadBoundary boundary;

	// 街区地址
	std::pair<std::string, int> address;

	// 内部园区
	OBJECT_HOLDER std::unordered_map<std::string, Zone*> zones;

	// 内部建筑
	OBJECT_HOLDER std::unordered_map<std::string, Building*> buildings;
};

