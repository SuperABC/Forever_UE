#pragma once

#include "utility.h"
#include "quad.h"

#include <vector>
#include <unordered_map>


class Roadnet;

class Node {
public:
	Node(float x, float y);
	~Node() = default;

	float GetX() const;
	float GetY() const;

private:
	float posX, posY;
};

class Connection {
public:
	Connection(std::string name, Roadnet* roadnet, int n1, int n2, float width = 1.0f, float begin = 0.0f, float end = 1.0f);
	~Connection() = default;

	bool operator==(Connection& other) const;

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

enum AREA_TYPE {
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

	// 获取/设置属性
	float GetRotation() const;
	void SetRotation(float r);
	AREA_TYPE GetArea() const;
	void SetArea(AREA_TYPE area);
	std::vector<std::pair<Connection, float>> GetRoads();
	void SetRoads(std::vector<std::pair<Connection, float>> roads);

	// 世界坐标变换
	std::pair<float, float> GetVertex(int idx) const;
	std::pair<float, float> GetPosition(float x, float y) const;

	// 通过逆时针顺序三个顶点设置矩形
	void SetPosition(Node n1, Node n2, Node n3, std::vector<float> margin);

	// 通过顺序无关四个顶点设置矩形
	void SetPosition(Node n1, Node n2, Node n3, Node n4, std::vector<float> margin);

protected:
	float rotation;
	AREA_TYPE area = AREA_GREEN;

	std::vector<std::pair<Connection, float>> roads;
};
