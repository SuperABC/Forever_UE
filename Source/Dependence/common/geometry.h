#pragma once

#include "utility.h"

#include <vector>


class Node {
public:
	// 禁止默认构造
	Node() = delete;

	// 显式构造分配新id
	explicit Node(float x, float y, float z = 0.f);

	// 拷贝构造使用相同id
	Node(const Node& other);

	// 赋值构造使用相同id
	Node& operator=(const Node& other);

	// 无析构
	~Node();

	// 获取id
	int GetId() const;

	// 获取坐标x
	float GetX() const;

	// 获取坐标y
	float GetY() const;

	// 获取坐标z
	float GetZ() const;

private:
	int id;
	float posX, posY, posZ;

	static int count;
};

class Connection {
public:
	// 禁止默认构造
	Connection() = delete;

	// 构造连接
	explicit Connection(std::string name, Node n1, Node n2, float radius = 1.0f, float begin = 0.0f, float end = 1.0f);

	// 拷贝构造新创建nodes
	Connection(const Connection& other);

	// 赋值构造新创建nodes
	Connection& operator=(const Connection& other);

	// 无析构
	~Connection();

	// 添加控制点
	void AddControls(std::vector<std::pair<Node, float>> controls);

	// 根据name, n1.id, n2.id判断是否相同
	bool operator==(const Connection& other) const;

	// 获取连接名称
	std::string GetName() const;

	// 获取start对应的起点（非实际端点)
	Node GetStart() const;

	// 获取end对应的终点（非实际端点)
	Node GetEnd() const;

	// 获取任一连线点
	Node GetPoint(float f) const;
	float GetRadius() const;

	// 获取begin和end之间的距离（非两端点距离)
	float CalcDistance() const;

	// 获连线任一两点距离
	float CalcDistance(float f1, float f2) const;

private:
	std::string name;

	OBJECT_HOLDER Node* beginVertex;
	OBJECT_HOLDER Node* endVertex;
	float begin, end;
	float radius;

	OBJECT_HOLDER std::vector<std::pair<Node*, float>> controlVertices;
};

class Quad {
public:
	// 构造空矩形
	Quad();

	// 构造矩形
	Quad(float x, float y, float w, float h);

	// 无析构
	virtual ~Quad();

	// 获取中心点坐标x
	float GetPosX() const;

	// 设置中心点坐标x
	void SetPosX(float x);

	// 获取中心点坐标y
	float GetPosY() const;

	// 设置中心点坐标y
	void SetPosY(float y);

	// 获取长宽尺寸x
	float GetSizeX() const;

	// 设置长宽尺寸x
	void SetSizeX(float w);

	// 获取长宽尺寸y
	float GetSizeY() const;

	// 设置长宽尺寸y
	void SetSizeY(float h);

	// 获取左边坐标x
	float GetLeft() const;

	// 获取右边坐标x
	float GetRight() const;

	// 获取下边坐标y
	float GetBottom() const;

	// 获取上边坐标y
	float GetTop() const;

	// 通过四边坐标指定位置
	void SetVertices(float x1, float y1, float x2, float y2);

	// 通过中心和尺寸指定位置
	void SetPosition(float x, float y, float w, float h);

	// 获取面积
	float GetAcreage() const;

	// 设置面积（在指定坐标之前设置期望面积）
	void SetAcreage(float a);

protected:
	float posX, posY;
	float sizeX, sizeY;

	float acreage;
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
	AREA_END
};

class Lot : public Quad {
public:
	// 构造空地块
	Lot();

	// 根据中心点和长宽构造地块
	Lot(float x, float y, float w, float h, float r);

	// 根据连续三个端点构造地块
	Lot(Node n1, Node n2, Node n3, std::vector<float> margin = std::vector<float>(4, 0.f));

	// 根据连续四个端点构造地块
	Lot(Node n1, Node n2, Node n3, Node n4, std::vector<float> margin = std::vector<float>(4, 0.f));

	// 无析构
	virtual ~Lot();

	// 获取旋转
	float GetRotation() const;

	// 设置旋转
	void SetRotation(float r);

	// 获取地块类型
	AREA_TYPE GetArea() const;

	// 设置地块类型
	void SetArea(AREA_TYPE area);

	// 获取端点坐标
	std::pair<float, float> GetVertex(int idx) const;

	// 获取矩形内任意点坐标
	std::pair<float, float> GetPosition(float x, float y) const;

	// 通过逆时针顺序三个顶点设置矩形
	void SetPosition(Node n1, Node n2, Node n3, const std::vector<float>& margin);

	// 通过顺序无关四个顶点设置矩形
	void SetPosition(Node n1, Node n2, Node n3, Node n4, const std::vector<float>& margin);

protected:
	float rotation;
	AREA_TYPE area;
};

