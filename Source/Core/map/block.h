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
	// 构造空街区
	Block();

	// 根据地块构造街区
	Block(Lot lot);

	// 析构园区与建筑
	virtual ~Block();

	// 获取街区相连路线及连接点
	const std::vector<std::pair<Road*, Node*>>& GetRoads() const;

	// 设置街区相连路线及连接点位置（按位置创建并持有连接点Node）
	void SetRoads(const std::vector<std::pair<Road*, float>>& roads);

	// 随机获取一个相连道路连接点
	Node* GetRandomRoadNode() const;

	// 获取全部园区
	std::unordered_map<std::string, Zone*>& GetZones();

	// 获取全部建筑
	std::unordered_map<std::string, Building*>& GetBuildings();

	// 添加一个园区
	void AddZone(const std::string& name, Zone* zone);

	// 添加一个建筑
	void AddBuilding(const std::string& name, Building* building);

	// 按名称获取园区
	Zone* GetZone(const std::string& name) const;

	// 按名称获取建筑
	Building* GetBuilding(const std::string& name) const;

	// 按名称移除园区
	void RemoveZone(const std::string& name);

	// 按名称移除建筑
	void RemoveBuilding(const std::string& name);

	// 设置街区地址
	void SetAddress(const std::string& road, int id);

	// 获取街区地址
	std::string GetAddress() const;

protected:
	// 相连道路
	OBJECT_HOLDER std::vector<std::pair<Road*, Node*>> roads;

	// 街区地址
	std::pair<std::string, int> address;

	// 内部园区
	OBJECT_HOLDER std::unordered_map<std::string, Zone*> zones;

	// 内部建筑
	OBJECT_HOLDER std::unordered_map<std::string, Building*> buildings;
};

