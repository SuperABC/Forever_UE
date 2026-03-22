#include "geometry.h"
#include "utility.h"
#include "error.h"

#include <cmath>
#include <string>
#include <vector>
#include <algorithm>


using namespace std;

int Node::count = 0;

Node::Node(float x, float y, float z) : id(count++), posX(x), posY(y), posZ(z) {

}

Node::~Node() {

}

int Node::GetId() const {
    return id;
}

float Node::GetX() const {
    return posX;
}

float Node::GetY() const {
    return posY;
}

float Node::GetZ() const {
    return posZ;
}

Node::Node(const Node& other) : id(other.id), posX(other.posX), posY(other.posY), posZ(other.posZ) {

}

Node& Node::operator=(const Node& other) {
    if (this != &other) {
        posX = other.posX;
        posY = other.posY;
        posZ = other.posZ;
        id = other.id;
    }
    return *this;
}

static int binomial(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    long long res = 1;
    for (int i = 1; i <= k; ++i) {
        res = res * (n - i + 1) / i;
    }
    return static_cast<int>(res);
}

static double bernstein(int n, int i, double t) {
    return binomial(n, i) * pow(t, i) * pow(1.0 - t, n - i);
}

Connection::Connection(string name, Node n1, Node n2, float radius, float begin, float end)
    : name(name), radius(radius), begin(begin), end(end),
    beginVertex(new Node(n1)), endVertex(new Node(n2)) {

}

Connection::Connection(const Connection& other) {
	name = other.name;
	radius = other.radius;
	begin = other.begin;
	end = other.end;
	beginVertex = new Node(*other.beginVertex);
	endVertex = new Node(*other.endVertex);
}

Connection& Connection::operator=(const Connection& other) {
	if (this != &other) {
		name = other.name;
		radius = other.radius;
		begin = other.begin;
		end = other.end;
		beginVertex = new Node(*other.beginVertex);
		endVertex = new Node(*other.endVertex);
	}
	return *this;
}

Connection::~Connection() {
    delete beginVertex;
    delete endVertex;
    for (auto& p : controlVertices) {
        delete p.first;
    }
}

void Connection::AddControls(vector<pair<Node, float>> controls) {
    for (auto& pair : controls) {
        controlVertices.emplace_back(new Node(pair.first), pair.second);
    }
}

bool Connection::operator==(const Connection& other) const {
    return name == other.name &&
        beginVertex->GetId() == other.beginVertex->GetId() &&
        endVertex->GetId() == other.endVertex->GetId();
}

string Connection::GetName() const {
    return name;
}

Node Connection::GetStart() const {
    return GetPoint(begin);
}

Node Connection::GetEnd() const {
    return GetPoint(end);
}

Node Connection::GetPoint(float f) const {
    if (f < 0.f || f > 1.f) {
        debugf("Warning: Bizier query position out of [0, 1].\n");
        if (f < 0.f)f = 0.f;
        if (f > 1.f)f = 1.f;
    }

    float x1 = beginVertex->GetX();
    float y1 = beginVertex->GetY();
    float z1 = beginVertex->GetZ();
    float x2 = endVertex->GetX();
    float y2 = endVertex->GetY();
    float z2 = endVertex->GetZ();

    if (controlVertices.empty()) {
        float x = x1 + f * (x2 - x1);
        float y = y1 + f * (y2 - y1);
        float z = z1 + f * (z2 - z1);
        return Node(x, y, z);
    }
    else {
        int n = static_cast<int>(controlVertices.size());
        vector<pair<Node*, float>> allPoints;
        allPoints.reserve(n + 2);
        allPoints.emplace_back(beginVertex, 1.0f);
        for (auto& p : controlVertices) {
            allPoints.push_back(p);
        }
        allPoints.emplace_back(endVertex, 1.0f);

        int m = static_cast<int>(allPoints.size()) - 1;
        double sumWeight = 0.0;
        double x = 0.0, y = 0.0, z = 0.0;

        for (int i = 0; i <= m; ++i) {
            double B = bernstein(m, i, f);
            double w = allPoints[i].second;
            sumWeight += B * w;
            x += B * w * allPoints[i].first->GetX();
            y += B * w * allPoints[i].first->GetY();
            z += B * w * allPoints[i].first->GetZ();
        }

        if (sumWeight != 0.0) {
            x /= sumWeight;
            y /= sumWeight;
            z /= sumWeight;
        }
        return Node(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
    }
}

float Connection::GetRadius() const {
    return radius;
}

static float ComputeLength(const Connection& connection, float t1, float t2, int segments = 16) {
    if (t1 == t2) return 0.0f;
    if (t1 > t2) swap(t1, t2);
    float step = (t2 - t1) / segments;
    float length = 0.0f;
    Node prev = connection.GetPoint(t1);
    for (int i = 1; i <= segments; ++i) {
        float t = t1 + i * step;
        Node curr = connection.GetPoint(t);
        float dx = curr.GetX() - prev.GetX();
        float dy = curr.GetY() - prev.GetY();
        float dz = curr.GetZ() - prev.GetZ();
        length += sqrt(dx * dx + dy * dy + dz * dz);
        prev = curr;
    }
    return length;
}

float Connection::CalcDistance() const {
    return ComputeLength(*this, begin, end);
}

float Connection::CalcDistance(float f1, float f2) const {
    return ComputeLength(*this, f1, f2);
}

Quad::Quad() : posX(0.f), posY(0.f), sizeX(0.f), sizeY(0.f), acreage(0.f) {

}

Quad::Quad(float x, float y, float w, float h) : posX(x), posY(y), sizeX(w), sizeY(h), acreage(w* h * 100.f) {

}

Quad::~Quad() {

}

float Quad::GetPosX() const {
    return posX;
}

void Quad::SetPosX(float x) {
    posX = x;
}

float Quad::GetPosY() const {
    return posY;
}

void Quad::SetPosY(float y) {
    posY = y;
}

float Quad::GetSizeX() const {
    return sizeX;
}

void Quad::SetSizeX(float w) {
    sizeX = w;
}

float Quad::GetSizeY() const {
    return sizeY;
}

void Quad::SetSizeY(float h) {
    sizeY = h;
}

float Quad::GetLeft() const {
    return posX - sizeX / 2.f;
}

float Quad::GetRight() const {
    return posX + sizeX / 2.f;
}

float Quad::GetBottom() const {
    return posY - sizeY / 2.f;
}

float Quad::GetTop() const {
    return posY + sizeY / 2.f;
}

void Quad::SetVertices(float x1, float y1, float x2, float y2) {
    if (x1 > x2) {
        swap(x1, x2);
    }
    if (y1 > y2) {
        swap(y1, y2);
    }

    posX = (x1 + x2) / 2.f;
    posY = (y1 + y2) / 2.f;
    sizeX = x2 - x1;
    sizeY = y2 - y1;
    acreage = sizeX * sizeY * 100.f;
}

void Quad::SetPosition(float x, float y, float w, float h) {
    posX = x;
    posY = y;
    sizeX = w;
    sizeY = h;
    acreage = sizeX * sizeY * 100.f;
}

float Quad::GetAcreage() const {
    return acreage;
}

void Quad::SetAcreage(float a) {
    acreage = a;
}

Lot::Lot() :
	Quad(), rotation(0.f), area(AREA_GREEN) {

}

Lot::Lot(float x, float y, float w, float h, float r) :
	Quad(x, y, w, h), rotation(r), area(AREA_GREEN) {

}

Lot::Lot(Node n1, Node n2, Node n3, vector<float> margin) :
	Quad(), rotation(0.f), area(AREA_GREEN) {
	SetPosition(n1, n2, n3, margin);
}

Lot::Lot(Node n1, Node n2, Node n3, Node n4, vector<float> margin) :
	Quad(), rotation(0.f), area(AREA_GREEN) {
	SetPosition(n1, n2, n3, n4, margin);
}

Lot::~Lot() {

}

float Lot::GetRotation() const {
	return rotation;
}

void Lot::SetRotation(float r) {
	rotation = r;
}

AREA_TYPE Lot::GetArea() const {
	return area;
}

void Lot::SetArea(AREA_TYPE a) {
	area = a;
}

pair<float, float> Lot::GetVertex(int idx) const {
	if (idx < 1 || idx > 4) {
		THROW_EXCEPTION(OutOfRangeException, "Block vertex out of range [0, 3].\n");
	}

	float hx = sizeX / 2.0f;
	float hy = sizeY / 2.0f;
	float c = cos(rotation);
	float s = sin(rotation);

	switch (idx) {
	case 1: // 右上
		return { posX + hx * c - hy * s, posY + hx * s + hy * c };
	case 2: // 左上
		return { posX - hx * c - hy * s, posY - hx * s + hy * c };
	case 3: // 左下
		return { posX - hx * c + hy * s, posY - hx * s - hy * c };
	case 4: // 右下
		return { posX + hx * c + hy * s, posY + hx * s - hy * c };
	default:
		return { 0.f, 0.f };
	}
}

pair<float, float> Lot::GetPosition(float x, float y) const {
	float cosR = cos(rotation);
	float sinR = sin(rotation);

	float relativeX = x - sizeX / 2.0f;
	float relativeY = y - sizeY / 2.0f;

	float rotatedX = relativeX * cosR - relativeY * sinR;
	float rotatedY = relativeX * sinR + relativeY * cosR;

	return { posX + rotatedX, posY + rotatedY };
}

void Lot::SetPosition(Node n1, Node n2, Node n3, const vector<float>& margin) {
	if (margin.size() != 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Block must have 4 margins.\n");
	}

	float x1 = n1.GetX(), y1 = n1.GetY();
	float x2 = n2.GetX(), y2 = n2.GetY();
	float x3 = n3.GetX(), y3 = n3.GetY();

	// 向量 u = p2 - p1, v = p3 - p2
	float ux = x2 - x1, uy = y2 - y1;
	float vx = x3 - x2, vy = y3 - y2;

	// 检查垂直
	float dot = ux * vx + uy * vy;
	const float eps = 1e-5f;
	if (abs(dot) > eps) {
		THROW_EXCEPTION(InvalidArgumentException, "Block edges are not perpendicular.\n");
	}

	// 计算尺寸
	float sx = sqrt(ux * ux + uy * uy);
	float sy = sqrt(vx * vx + vy * vy);

	// 应用边距
	sx -= margin[1] + margin[3];
	sy -= margin[0] + margin[2];

	// 计算中心点：p1 和 p3 的对角线中心，加上边距偏移
	float cx = (x1 + x3) / 2.0f
		+ (margin[3] - margin[1]) * ux / (2.0f * sx)
		+ (margin[2] - margin[0]) * vx / (2.0f * sy);
	float cy = (y1 + y3) / 2.0f
		+ (margin[3] - margin[1]) * uy / (2.0f * sx)
		+ (margin[2] - margin[0]) * vy / (2.0f * sy);

	// 计算旋转角度（p1p2 与 x 轴夹角）
	float rot = atan2(uy, ux);

	// 更新成员变量
	posX = cx;
	posY = cy;
	sizeX = sx;
	sizeY = sy;
	rotation = rot;
	acreage = sx * sy * 100.f;
}

void Lot::SetPosition(Node n1, Node n2, Node n3, Node n4, const vector<float>& margin) {
	if (margin.size() != 4) {
		THROW_EXCEPTION(InvalidArgumentException, "Block must have 4 margins.\n");
	}

	vector<Node> nodes = { n1, n2, n3, n4 };

	// 计算中心点
	float cx = 0.0f, cy = 0.0f;
	for (const auto& node : nodes) {
		cx += node.GetX();
		cy += node.GetY();
	}
	cx /= 4.0f;
	cy /= 4.0f;

	// 按顺时针排序
	sort(nodes.begin(), nodes.end(),
		[cx, cy](const Node& a, const Node& b) {
			return atan2(a.GetY() - cy, a.GetX() - cx) >
				atan2(b.GetY() - cy, b.GetX() - cx);
		});

	// 检查矩形条件
	const float eps = 1e-2f;

	// 计算四个边向量
	float u1x = nodes[1].GetX() - nodes[0].GetX();
	float u1y = nodes[1].GetY() - nodes[0].GetY();
	float u2x = nodes[2].GetX() - nodes[1].GetX();
	float u2y = nodes[2].GetY() - nodes[1].GetY();
	float u3x = nodes[3].GetX() - nodes[2].GetX();
	float u3y = nodes[3].GetY() - nodes[2].GetY();
	float u4x = nodes[0].GetX() - nodes[3].GetX();
	float u4y = nodes[0].GetY() - nodes[3].GetY();

	// 检查邻边垂直（点积为0）
	float dot1 = u1x * u2x + u1y * u2y;
	float dot2 = u2x * u3x + u2y * u3y;
	float dot3 = u3x * u4x + u3y * u4y;
	float dot4 = u4x * u1x + u4y * u1y;

	if (abs(dot1) > eps || abs(dot2) > eps ||
		abs(dot3) > eps || abs(dot4) > eps) {
		THROW_EXCEPTION(InvalidArgumentException, "Block edges are not perpendicular.\n");
	}

	// 检查对边长度相等
	float len1 = sqrt(u1x * u1x + u1y * u1y);
	float len2 = sqrt(u2x * u2x + u2y * u2y);
	float len3 = sqrt(u3x * u3x + u3y * u3y);
	float len4 = sqrt(u4x * u4x + u4y * u4y);

	if (abs(len1 - len3) > eps || abs(len2 - len4) > eps) {
		THROW_EXCEPTION(InvalidArgumentException, "Block opposite edges are not equal.\n");
	}

	// 计算尺寸（取相邻两边长度）
	float sx = len1;
	float sy = len2;

	// 计算旋转角度（使用第一条边 nodes[0]->nodes[1]）
	float rot = atan2(u1y, u1x);

	// 应用边距
	sx -= margin[1] + margin[3];
	sy -= margin[0] + margin[2];
	cx += (margin[3] - margin[1]) * u1x / (2.0f * sx)
		+ (margin[2] - margin[0]) * u2x / (2.0f * sy);
	cy += (margin[3] - margin[1]) * u1y / (2.0f * sx)
		+ (margin[2] - margin[0]) * u2y / (2.0f * sy);

	// 更新成员变量
	posX = cx;
	posY = cy;
	sizeX = sx;
	sizeY = sy;
	rotation = rot;
	acreage = sx * sy * 100.f;
}
