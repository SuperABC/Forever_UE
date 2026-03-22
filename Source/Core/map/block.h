#pragma once

#include "zone.h"
#include "building.h"
#include "geometry.h"

#include <vector>
#include <unordered_map>


class Zone;
//class Building;
class Block : public Lot {
public:
	// 构造空街区
	Block();

	// 根据地块构造街区
	Block(Lot lot);

	// 析构园区与建筑
	virtual ~Block();

	// 获取街区相连路线及连接点位置
	const std::vector<std::pair<Connection*, float>>& GetRoads() const;

	// 设置街区相连路线及连接点位置
	void SetRoads(const std::vector<std::pair<Connection*, float>>& roads);

	// 内部Quad管理
	std::unordered_map<std::string, Zone*>& GetZones();
	//std::unordered_map<std::string, Building*>& GetBuildings();
	void AddZone(const std::string& name, Zone* zone);
	//void AddBuilding(const std::string& name, Building* building);
	Zone* GetZone(const std::string& name) const;
	//Building* GetBuilding(const std::string& name) const;
	void RemoveZone(const std::string& name);
	//void RemoveBuilding(const std::string& name);

	// 寻址
	void SetAddress(const std::string& road, int id);
	std::string GetAddress() const;

protected:
	VARIABLE_HOLDER std::vector<std::pair<Connection*, float>> roads;
	std::pair<std::string, int> address;

	VARIABLE_HOLDER std::unordered_map<std::string, Zone*> zones;
	//VARIABLE_HOLDER std::unordered_map<std::string, Building*> buildings;
};

