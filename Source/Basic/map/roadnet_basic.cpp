#include "roadnet_basic.h"

#define ROAD_CONTROL_POINT_RATIO 0.33f
#define TUNNEL_HEIGHT -1.f
#define TUNNEL_HATCH_WIDTH 1.f
#define TUNNEL_HATCH_LENGTH 2.f


using namespace std;

int JingRoadnet::count = 0;

JingRoadnet::JingRoadnet() : id(count++) {

}

JingRoadnet::~JingRoadnet() {

}

const char* JingRoadnet::GetId() {
	return "jing";
}

const char* JingRoadnet::GetType() const {
	return "jing";
}

const char* JingRoadnet::GetName() {
	name = "井字路网" + to_string(id);
	return name.data();
}

void JingRoadnet::DistributeRoadnet(int width, int height,
	const function<string(int, int)>& getTerrain,
	const function<float(int, int)>& getHeight,
	const function<pair<bool, float>(int, int)>& getWater, int nodeStaticCount) {
	Node::SetCount(nodeStaticCount);

	string meshPath = "/Game/Asset/Meshes/default_1_1.default_1_1";

	vector<pair<Node, int>> horizontalNode1w;
	vector<pair<Node, int>> horizontalNode1e;
	vector<pair<Node, int>> horizontalNode2w;
	vector<pair<Node, int>> horizontalNode2e;
	vector<pair<Node, int>> verticalNode1n;
	vector<pair<Node, int>> verticalNode1s;
	vector<pair<Node, int>> verticalNode2n;
	vector<pair<Node, int>> verticalNode2s;

	// 按地形/水域计算某坐标处道路应有的高度：水域用水面+1（架桥），否则用地形本身高度
	auto sampleHeight = [&](float x, float y) -> float {
		auto [isWater, waterLevel] = getWater(static_cast<int>(x), static_cast<int>(y));
		return isWater ? waterLevel + 1.f : getHeight(static_cast<int>(x), static_cast<int>(y));
		};

	// 沿给定方向延伸链条直到地图边界；山体节点直接按隧道高度处理，山体node本身就是隧道的起点/终点
	auto extendChain = [&](float startX, float startY, float stepX, float stepY, vector<pair<Node, int>>& chain) {
		for (float x = startX, y = startY; ; x += stepX, y += stepY) {
			string t = getTerrain(static_cast<int>(x), static_cast<int>(y));
			if (t == "") break;
			float h = (t == "mountain") ? TUNNEL_HEIGHT : sampleHeight(x, y);
			intersections.emplace_back(x, y, h);
			chain.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
		}
		};

	// 建一条道路：普通地面段控制点采样真实地形；隧道入口/出口过渡段从地面端朝隧道端开一个长度为
	// TUNNEL_HATCH_LENGTH的hatch，3个控制点全部落在hatch范围内做出S形平滑坡度；完全在隧道内的路段保持平直
	auto addRoad = [&](const string& name, const Node& n1, const Node& n2) {
		roads.emplace_back(name, n1, n2, meshPath);

		bool n1Tunnel = n1.GetZ() < 0.f;
		bool n2Tunnel = n2.GetZ() < 0.f;

		if (n1Tunnel != n2Tunnel) {
			const Node& groundNode = n1Tunnel ? n2 : n1;
			const Node& tunnelNode = n1Tunnel ? n1 : n2;
			float dx = tunnelNode.GetX() - groundNode.GetX();
			float dy = tunnelNode.GetY() - groundNode.GetY();
			float segLen = sqrt(dx * dx + dy * dy);
			float ux = dx / segLen, uy = dy / segLen;

			// 从地面端(groundNode)朝隧道端方向量出长度为TUNNEL_HATCH_LENGTH的hatch，3个控制点都落在
			// 这1米范围内：c1贴近groundNode且高度跟它一样（标准S形曲线的起点侧控制点，保证起坡处切线水平），
			// c2高度已经是隧道高度（S形曲线的下坠点），c3就卡在hatch终点(第1米末尾)、高度同样是隧道高度，
			// 当"斜坡终点"用——从c3开始一直到tunnelNode都维持隧道高度，不再变化
			float c1x = groundNode.GetX() + ux * TUNNEL_HATCH_LENGTH / 3.f;
			float c1y = groundNode.GetY() + uy * TUNNEL_HATCH_LENGTH / 3.f;
			float c2x = groundNode.GetX() + ux * TUNNEL_HATCH_LENGTH * 2.f / 3.f;
			float c2y = groundNode.GetY() + uy * TUNNEL_HATCH_LENGTH * 2.f / 3.f;
			float c3x = groundNode.GetX() + ux * TUNNEL_HATCH_LENGTH;
			float c3y = groundNode.GetY() + uy * TUNNEL_HATCH_LENGTH;
			Node c1("roadnet", c1x, c1y, groundNode.GetZ());
			Node c2("roadnet", c2x, c2y, tunnelNode.GetZ());
			Node c3("roadnet", c3x, c3y, tunnelNode.GetZ());
			if (n1Tunnel) {
				roads.back().AddControls({ { c3, 1.f }, { c2, 1.f }, { c1, 1.f } });
			}
			else {
				roads.back().AddControls({ { c1, 1.f }, { c2, 1.f }, { c3, 1.f } });
			}

			// hatch从groundNode开始，沿隧道方向开TUNNEL_HATCH_LENGTH；Connection::GetPoint的参数
			// 现在是按弧长线性变化的，spanT可以直接按比例换算，不需要再手动二分查找弧长
			float spanT = TUNNEL_HATCH_LENGTH / segLen;
			if (n1Tunnel) {
				AddHatch(&roads.back(), 1.f - spanT, 1.f, TUNNEL_HATCH_WIDTH);
			}
			else {
				AddHatch(&roads.back(), 0.f, spanT, TUNNEL_HATCH_WIDTH);
			}
		}
		else {
			float cx = n1.GetX() + (n2.GetX() - n1.GetX()) * ROAD_CONTROL_POINT_RATIO;
			float cy = n1.GetY() + (n2.GetY() - n1.GetY()) * ROAD_CONTROL_POINT_RATIO;
			float cz = (n1Tunnel && n2Tunnel) ? TUNNEL_HEIGHT : sampleHeight(cx, cy);
			roads.back().AddControls({ { Node("roadnet", cx, cy, cz), 1.f } });
		}
		};

	// 判断某坐标地形是否可以铺设block（plain或construction）
	auto isBuildable = [&](float x, float y) -> bool {
		string t = getTerrain(static_cast<int>(x), static_cast<int>(y));
		return t == "plain" || t == "construction";
		};

	float theta = GetRandom(1000) / 1000.f * 0.4f - 0.2f;

	float nwX = width / 2.f + 16.f * (sin(theta) - cos(theta)), nwY = height / 2.f + 16.f * (-cos(theta) - sin(theta));
	float neX = width / 2.f + 16.f * (sin(theta) + cos(theta)), neY = height / 2.f + 16.f * (-cos(theta) + sin(theta));
	float seX = width / 2.f + 16.f * (-sin(theta) + cos(theta)), seY = height / 2.f + 16.f * (cos(theta) + sin(theta));
	float swX = width / 2.f + 16.f * (-sin(theta) - cos(theta)), swY = height / 2.f + 16.f * (cos(theta) - sin(theta));
	Intersection northWest(nwX, nwY, sampleHeight(nwX, nwY));
	Intersection northEast(neX, neY, sampleHeight(neX, neY));
	Intersection southEast(seX, seY, sampleHeight(seX, seY));
	Intersection southWest(swX, swY, sampleHeight(swX, swY));
	intersections.push_back(northWest);
	intersections.push_back(northEast);
	intersections.push_back(southEast);
	intersections.push_back(southWest);

	float dirSin = northEast.GetY() - northWest.GetY();
	float dirCos = northEast.GetX() - northWest.GetX();

	extendChain(northWest.GetX() - dirCos, northWest.GetY() - dirSin, -dirCos, -dirSin, horizontalNode1w);
	intersections.pop_back();
	extendChain(northEast.GetX() + dirCos, northEast.GetY() + dirSin, dirCos, dirSin, horizontalNode1e);
	intersections.pop_back();
	extendChain(southWest.GetX() - dirCos, southWest.GetY() - dirSin, -dirCos, -dirSin, horizontalNode2w);
	intersections.pop_back();
	extendChain(southEast.GetX() + dirCos, southEast.GetY() + dirSin, dirCos, dirSin, horizontalNode2e);
	intersections.pop_back();
	extendChain(northWest.GetX() + dirSin, northWest.GetY() - dirCos, dirSin, -dirCos, verticalNode1n);
	intersections.pop_back();
	extendChain(southWest.GetX() - dirSin, southWest.GetY() + dirCos, -dirSin, dirCos, verticalNode1s);
	intersections.pop_back();
	extendChain(northEast.GetX() + dirSin, northEast.GetY() - dirCos, dirSin, -dirCos, verticalNode2n);
	intersections.pop_back();
	extendChain(southEast.GetX() - dirSin, southEast.GetY() + dirCos, -dirSin, dirCos, verticalNode2s);
	intersections.pop_back();
	externs.emplace_back(horizontalNode1w.back().first);
	horizontalNode1w.pop_back();
	externs.emplace_back(horizontalNode1e.back().first);
	horizontalNode1e.pop_back();
	externs.emplace_back(horizontalNode2w.back().first);
	horizontalNode2w.pop_back();
	externs.emplace_back(horizontalNode2e.back().first);
	horizontalNode2e.pop_back();
	externs.emplace_back(verticalNode1n.back().first);
	verticalNode1n.pop_back();
	externs.emplace_back(verticalNode1s.back().first);
	verticalNode1s.pop_back();
	externs.emplace_back(verticalNode2n.back().first);
	verticalNode2n.pop_back();
	externs.emplace_back(verticalNode2s.back().first);
	verticalNode2s.pop_back();

	addRoad("中山西路", intersections[0], intersections[3]);
	addRoad("中山东路", intersections[1], intersections[2]);
	addRoad("中山北路", intersections[0], intersections[1]);
	addRoad("中山南路", intersections[3], intersections[2]);

	if (horizontalNode1w.size() > 0) {
		addRoad("城西北路", intersections[0], intersections[horizontalNode1w[0].second]);
	}
	for (size_t i = 1; i < horizontalNode1w.size(); i++) {
		const auto& [node1, idx1] = horizontalNode1w[i];
		const auto& [node2, idx2] = horizontalNode1w[i - 1];
		addRoad("城西北路", intersections[idx1], intersections[idx2]);
	}
	addRoad("城西北路", intersections[horizontalNode1w.back().second], externs[0]);
	if (horizontalNode1e.size() > 0) {
		addRoad("城东北路", intersections[1], intersections[horizontalNode1e[0].second]);
	}
	for (size_t i = 1; i < horizontalNode1e.size(); i++) {
		const auto& [node1, idx1] = horizontalNode1e[i];
		const auto& [node2, idx2] = horizontalNode1e[i - 1];
		addRoad("城东北路", intersections[idx1], intersections[idx2]);
	}
	addRoad("城东北路", intersections[horizontalNode1e.back().second], externs[1]);
	if (horizontalNode2w.size() > 0) {
		addRoad("城西南路", intersections[3], intersections[horizontalNode2w[0].second]);
	}
	for (size_t i = 1; i < horizontalNode2w.size(); i++) {
		const auto& [node1, idx1] = horizontalNode2w[i];
		const auto& [node2, idx2] = horizontalNode2w[i - 1];
		addRoad("城西南路", intersections[idx1], intersections[idx2]);
	}
	addRoad("城西南路", intersections[horizontalNode2w.back().second], externs[2]);
	if (horizontalNode2e.size() > 0) {
		addRoad("城东南路", intersections[2], intersections[horizontalNode2e[0].second]);
	}
	for (size_t i = 1; i < horizontalNode2e.size(); i++) {
		const auto& [node1, idx1] = horizontalNode2e[i];
		const auto& [node2, idx2] = horizontalNode2e[i - 1];
		addRoad("城东南路", intersections[idx1], intersections[idx2]);
	}
	addRoad("城东南路", intersections[horizontalNode2e.back().second], externs[3]);
	if (verticalNode1n.size() > 0) {
		addRoad("城北西路", intersections[0], intersections[verticalNode1n[0].second]);
	}
	for (size_t i = 1; i < verticalNode1n.size(); i++) {
		const auto& [node1, idx1] = verticalNode1n[i];
		const auto& [node2, idx2] = verticalNode1n[i - 1];
		addRoad("城北西路", intersections[idx1], intersections[idx2]);
	}
	addRoad("城北西路", intersections[verticalNode1n.back().second], externs[4]);
	if (verticalNode1s.size() > 0) {
		addRoad("城南西路", intersections[3], intersections[verticalNode1s[0].second]);
	}
	for (size_t i = 1; i < verticalNode1s.size(); i++) {
		const auto& [node1, idx1] = verticalNode1s[i];
		const auto& [node2, idx2] = verticalNode1s[i - 1];
		addRoad("城南西路", intersections[idx1], intersections[idx2]);
	}
	addRoad("城南西路", intersections[verticalNode1s.back().second], externs[5]);
	if (verticalNode2n.size() > 0) {
		addRoad("城北东路", intersections[1], intersections[verticalNode2n[0].second]);
	}
	for (size_t i = 1; i < verticalNode2n.size(); i++) {
		const auto& [node1, idx1] = verticalNode2n[i];
		const auto& [node2, idx2] = verticalNode2n[i - 1];
		addRoad("城北东路", intersections[idx1], intersections[idx2]);
	}
	addRoad("城北东路", intersections[verticalNode2n.back().second], externs[6]);
	if (verticalNode2s.size() > 0) {
		addRoad("城南东路", intersections[2], intersections[verticalNode2s[0].second]);
	}
	for (size_t i = 1; i < verticalNode2s.size(); i++) {
		const auto& [node1, idx1] = verticalNode2s[i];
		const auto& [node2, idx2] = verticalNode2s[i - 1];
		addRoad("城南东路", intersections[idx1], intersections[idx2]);
	}
	addRoad("城南东路", intersections[verticalNode2s.back().second], externs[7]);

	lots.emplace_back(
		Lot(northWest, northEast, southEast, southWest, { 0.5f, 0.5f, 0.5f, 0.5f }), make_pair(
			unordered_map<int, Road>{ {0, roads[0]}, {1, roads[1]}, {2, roads[2]}, {3, roads[3]} },
			unordered_map<int, Intersection>{ {0, intersections[0]}, {1, intersections[1]}, {2, intersections[2]}, {3, intersections[3]} }
		)
	);
	lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	//return;

	if (horizontalNode1w.size() >= 1 && horizontalNode2w.size() >= 1) {
		lots.emplace_back(Lot(horizontalNode1w[0].first, intersections[0], intersections[3], horizontalNode2w[0].first, { 0.0f, 0.5f, 0.5f, 0.5f }), make_pair(
		unordered_map<int, Road>{
			{ 1, roads[0] },
			{ 2, Road("城西北路", intersections[horizontalNode1w[0].second], intersections[0], meshPath) },
			{ 3, Road("城西南路", intersections[horizontalNode2w[0].second], intersections[3], meshPath) }
		},
		unordered_map<int, Intersection>{
			{ 0, intersections[horizontalNode1w[0].second] },
			{ 1, intersections[0] },
			{ 2, intersections[3] },
			{ 3, intersections[horizontalNode2w[0].second] }
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
	if (horizontalNode1e.size() >= 1 && horizontalNode2e.size() >= 1) {
		lots.emplace_back(Lot(intersections[1], horizontalNode1e[0].first, horizontalNode2e[0].first, intersections[2], { 0.5f, 0.0f, 0.5f, 0.5f }), make_pair(
		unordered_map<int, Road>{
			{ 0, roads[1] },
			{ 2, Road("城东北路", intersections[horizontalNode1e[0].second], intersections[1], meshPath) },
			{ 3, Road("城东南路", intersections[horizontalNode2e[0].second], intersections[2], meshPath) }
		},
		unordered_map<int, Intersection>{
			{ 0, intersections[1] },
			{ 1, intersections[horizontalNode1e[0].second] },
			{ 2, intersections[horizontalNode2e[0].second] },
			{ 3, intersections[2] }
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
	if (verticalNode1n.size() >= 1 && verticalNode2n.size() >= 1) {
		lots.emplace_back(Lot(verticalNode1n[0].first, verticalNode2n[0].first, intersections[1], intersections[0], { 0.5f, 0.5f, 0.0f, 0.5f }), make_pair(
		unordered_map<int, Road>{
			{ 0, Road("城北西路", intersections[verticalNode1n[0].second], intersections[0], meshPath) },
			{ 1, Road("城北东路", intersections[verticalNode2n[0].second], intersections[1], meshPath) },
			{ 3, roads[2] }
		},
		unordered_map<int, Intersection>{
			{ 0, intersections[verticalNode1n[0].second] },
			{ 1, intersections[verticalNode2n[0].second] },
			{ 2, intersections[1] },
			{ 3, intersections[0] }
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
	if (verticalNode1s.size() >= 1 && verticalNode2s.size() >= 1) {
		lots.emplace_back(Lot(intersections[3], intersections[2], verticalNode2s[0].first, verticalNode1s[0].first, { 0.5f, 0.5f, 0.5f, 0.0f }), make_pair(
		unordered_map<int, Road>{
			{ 0, Road("城南西路", intersections[verticalNode1s[0].second], intersections[3], meshPath) },
			{ 1, Road("城南东路", intersections[verticalNode2s[0].second], intersections[2], meshPath) },
			{ 2, roads[3] }
		},
		unordered_map<int, Intersection>{
			{ 0, intersections[3] },
			{ 1, intersections[2] },
			{ 2, intersections[verticalNode2s[0].second] },
			{ 3, intersections[verticalNode1s[0].second] }
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
	return;

	for (size_t i = 1; i < min(horizontalNode1w.size(), horizontalNode2w.size()); i++) {
		if (!isBuildable(horizontalNode1w[i].first.GetX(), horizontalNode1w[i].first.GetY())) break;
		if (!isBuildable(horizontalNode1w[i - 1].first.GetX(), horizontalNode1w[i - 1].first.GetY())) break;
		if (!isBuildable(horizontalNode2w[i].first.GetX(), horizontalNode2w[i].first.GetY())) break;
		if (!isBuildable(horizontalNode2w[i - 1].first.GetX(), horizontalNode2w[i - 1].first.GetY())) break;

		const auto& [nwNode, nwIdx] = horizontalNode1w[i];
		const auto& [neNode, neIdx] = horizontalNode1w[i - 1];
		const auto& [seNode, seIdx] = horizontalNode2w[i - 1];
		const auto& [swNode, swIdx] = horizontalNode2w[i];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.0f, 0.0f, 0.5f, 0.5f }), make_pair(
		unordered_map<int, Road>{
			{ 2, Road("城西北路", intersections[nwIdx], intersections[neIdx], meshPath) },
			{ 3, Road("城西南路", intersections[swIdx], intersections[seIdx], meshPath) }
		},
		unordered_map<int, Intersection>{
			{ 0, intersections[nwIdx] },
			{ 1, intersections[neIdx] },
			{ 2, intersections[seIdx] },
			{ 3, intersections[swIdx] }
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(horizontalNode1e.size(), horizontalNode2e.size()); i++) {
		if (!isBuildable(horizontalNode1e[i].first.GetX(), horizontalNode1e[i].first.GetY())) break;
		if (!isBuildable(horizontalNode1e[i - 1].first.GetX(), horizontalNode1e[i - 1].first.GetY())) break;
		if (!isBuildable(horizontalNode2e[i].first.GetX(), horizontalNode2e[i].first.GetY())) break;
		if (!isBuildable(horizontalNode2e[i - 1].first.GetX(), horizontalNode2e[i - 1].first.GetY())) break;

		const auto& [nwNode, nwIdx] = horizontalNode1e[i - 1];
		const auto& [neNode, neIdx] = horizontalNode1e[i];
		const auto& [seNode, seIdx] = horizontalNode2e[i];
		const auto& [swNode, swIdx] = horizontalNode2e[i - 1];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.0f, 0.0f, 0.5f, 0.5f }), make_pair(
			unordered_map<int, Road>{
				{ 2, Road("城东北路", intersections[nwIdx], intersections[neIdx], meshPath) },
				{ 3, Road("城东南路", intersections[swIdx], intersections[seIdx], meshPath) }
			},
			unordered_map<int, Intersection>{
				{ 0, intersections[nwIdx] },
				{ 1, intersections[neIdx] },
				{ 2, intersections[seIdx] },
				{ 3, intersections[swIdx] }
			}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(verticalNode1n.size(), verticalNode2n.size()); i++) {
		if (!isBuildable(verticalNode1n[i].first.GetX(), verticalNode1n[i].first.GetY())) break;
		if (!isBuildable(verticalNode1n[i - 1].first.GetX(), verticalNode1n[i - 1].first.GetY())) break;
		if (!isBuildable(verticalNode2n[i].first.GetX(), verticalNode2n[i].first.GetY())) break;
		if (!isBuildable(verticalNode2n[i - 1].first.GetX(), verticalNode2n[i - 1].first.GetY())) break;

		const auto& [nwNode, nwIdx] = verticalNode1n[i];
		const auto& [neNode, neIdx] = verticalNode2n[i];
		const auto& [seNode, seIdx] = verticalNode2n[i - 1];
		const auto& [swNode, swIdx] = verticalNode1n[i - 1];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.5f, 0.5f, 0.0f, 0.0f }), make_pair(
			unordered_map<int, Road>{
				{ 0, Road("城北西路", intersections[nwIdx], intersections[swIdx], meshPath) },
				{ 1, Road("城北东路", intersections[neIdx], intersections[seIdx], meshPath) }
			},
			unordered_map<int, Intersection>{
				{ 0, intersections[nwIdx] },
				{ 1, intersections[neIdx] },
				{ 2, intersections[seIdx] },
				{ 3, intersections[swIdx] }
			}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(verticalNode1s.size(), verticalNode2s.size()); i++) {
		if (!isBuildable(verticalNode1s[i].first.GetX(), verticalNode1s[i].first.GetY())) break;
		if (!isBuildable(verticalNode1s[i - 1].first.GetX(), verticalNode1s[i - 1].first.GetY())) break;
		if (!isBuildable(verticalNode2s[i].first.GetX(), verticalNode2s[i].first.GetY())) break;
		if (!isBuildable(verticalNode2s[i - 1].first.GetX(), verticalNode2s[i - 1].first.GetY())) break;

		const auto& [nwNode, nwIdx] = verticalNode1s[i - 1];
		const auto& [neNode, neIdx] = verticalNode2s[i - 1];
		const auto& [seNode, seIdx] = verticalNode2s[i];
		const auto& [swNode, swIdx] = verticalNode1s[i];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.5f, 0.5f, 0.0f, 0.0f }), make_pair(
			unordered_map<int, Road>{
				{ 0, Road("城南西路", intersections[nwIdx], intersections[swIdx], meshPath) },
				{ 1, Road("城南东路", intersections[neIdx], intersections[seIdx], meshPath) }
			},
			unordered_map<int, Intersection>{
				{ 0, intersections[nwIdx] },
				{ 1, intersections[neIdx] },
				{ 2, intersections[seIdx] },
				{ 3, intersections[swIdx] }
			}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
}
