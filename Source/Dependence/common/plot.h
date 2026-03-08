#pragma once

#include "../map/zone_base.h"
#include "../map/building_base.h"

#include "quad.h"

#include <vector>
#include <unordered_map>


class Roadnet;
class Zone;
class Building;

class Node {
public:
	Node(float x, float y);
	~Node();

	float GetX() const;
	float GetY() const;

private:
	float posX, posY;
};

class Connection {
public:
	Connection(std::string name, Roadnet* roadnet, int n1, int n2, float width = 1.0f, float begin = 0.0f, float end = 1.0f);
	~Connection();

	bool operator==(const Connection& other) const;

	std::string GetName() const;
	Roadnet* GetRoadnet() const;
	float GetWidth() const;

	int GetV1() const;
	int GetV2() const;
	float distance() const;

private:
	std::string name;
	Roadnet* roadnet;

	std::pair<int, int> vertices;
	float begin, end;
	float width;
};

enum AREA_TYPE : int {
	AREA_NONE,
	AREA_RESIDENTIAL_HIGH,
	AREA_RESIDENTIAL_MIDDLE,
	AREA_RESIDENTIAL_LOW,
	AREA_COMMERCIAL_HIGH,
	AREA_COMMERCIAL_MIDDLE,
	AREA_COMMERCIAL_LOW,
	AREA_INDUSTRIAL_HIGH,
	AREA_INDUSTRIAL_MIDDLE,
	AREA_INDUSTRIAL_LOW,
	AREA_OFFICIAL_HIGH,
	AREA_OFFICIAL_MIDDLE,
	AREA_OFFICIAL_LOW,
	AREA_GREEN,
};

class Plot : public Quad {
public:
	Plot();
	Plot(float x, float y, float w, float h, float r);
	Plot(Node n1, Node n2, Node n3, std::vector<float> margin = std::vector<float>(4, 0.f));
	Plot(Node n1, Node n2, Node n3, Node n4, std::vector<float> margin = std::vector<float>(4, 0.f));
	virtual ~Plot();

	// 获取/设置属性
	float GetRotation() const;
	void SetRotation(float r);
	AREA_TYPE GetArea() const;
	void SetArea(AREA_TYPE area);
	const std::vector<std::pair<Connection, float>>& GetRoads() const;
	void SetRoads(const std::vector<std::pair<Connection, float>>& roads);

	// 世界坐标变换
	std::pair<float, float> GetVertex(int idx) const;
	std::pair<float, float> GetPosition(float x, float y) const;

	// 通过逆时针顺序三个顶点设置矩形
	void SetPosition(Node n1, Node n2, Node n3, const std::vector<float>& margin);

	// 通过顺序无关四个顶点设置矩形
	void SetPosition(Node n1, Node n2, Node n3, Node n4, const std::vector<float>& margin);

	// 内部Quad管理
	std::unordered_map<std::string, Zone*>& GetZones();
	std::unordered_map<std::string, Building*>& GetBuildings();
	void AddZone(const std::string& name, Zone* zone);
	void AddBuilding(const std::string& name, Building* building);
	Zone* GetZone(const std::string& name) const;
	Building* GetBuilding(const std::string& name) const;
	void RemoveZone(const std::string& name);
	void RemoveBuilding(const std::string& name);

	// 寻址
	void SetAddress(const std::string& road, int id);
	std::string GetAddress() const;

protected:
	float rotation;
	AREA_TYPE area;

	std::pair<std::string, int> address;
	std::vector<std::pair<Connection, float>> roads;

	std::unordered_map<std::string, Zone*> zones;
	std::unordered_map<std::string, Building*> buildings;
};