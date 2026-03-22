#include "../common/error.h"
#include "../common/utility.h"

#include "roadnet_basic.h"


using namespace std;

// 井字路网

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
	function<string(int, int)> get) {
	vector<pair<Node, int>> horizontalNode1w;
	vector<pair<Node, int>> horizontalNode1e;
	vector<pair<Node, int>> horizontalNode2w;
	vector<pair<Node, int>> horizontalNode2e;
	vector<pair<Node, int>> verticalNode1n;
	vector<pair<Node, int>> verticalNode1s;
	vector<pair<Node, int>> verticalNode2n;
	vector<pair<Node, int>> verticalNode2s;

	float theta = GetRandom(1000) / 1000.f * 0.4f - 0.2f;

	Node northWest(width / 2.f + 16.f * (sin(theta) - cos(theta)), height / 2.f + 16.f * (-cos(theta) - sin(theta)));
	Node northEast(width / 2.f + 16.f * (sin(theta) + cos(theta)), height / 2.f + 16.f * (-cos(theta) + sin(theta)));
	Node southWest(width / 2.f + 16.f * (-sin(theta) - cos(theta)), height / 2.f + 16.f * (cos(theta) - sin(theta)));
	Node southEast(width / 2.f + 16.f * (-sin(theta) + cos(theta)), height / 2.f + 16.f * (cos(theta) + sin(theta)));
	nodes.push_back(northWest);
	nodes.push_back(northEast);
	nodes.push_back(southWest);
	nodes.push_back(southEast);

	float s = northEast.GetY() - northWest.GetY();
	float c = northEast.GetX() - northWest.GetX();

	for (float x = northWest.GetX(), y = northWest.GetY(); string(get((int)x, (int)y)) != ""; x -= c, y -= s) {
		nodes.emplace_back(x, y);
		horizontalNode1w.emplace_back(nodes.back(), (int)nodes.size() - 1);
	}
	for (float x = northEast.GetX(), y = northEast.GetY(); string(get((int)x, (int)y)) != ""; x += c, y += s) {
		nodes.emplace_back(x, y);
		horizontalNode1e.emplace_back(nodes.back(), (int)nodes.size() - 1);
	}
	for (float x = southWest.GetX(), y = southWest.GetY(); string(get((int)x, (int)y)) != ""; x -= c, y -= s) {
		nodes.emplace_back(x, y);
		horizontalNode2w.emplace_back(nodes.back(), (int)nodes.size() - 1);
	}
	for (float x = southEast.GetX(), y = southEast.GetY(); string(get((int)x, (int)y)) != ""; x += c, y += s) {
		nodes.emplace_back(x, y);
		horizontalNode2e.emplace_back(nodes.back(), (int)nodes.size() - 1);
	}
	for (float x = northWest.GetX(), y = northWest.GetY(); string(get((int)x, (int)y)) != ""; x += s, y -= c) {
		nodes.emplace_back(x, y);
		verticalNode1n.emplace_back(nodes.back(), (int)nodes.size() - 1);
	}
	for (float x = southWest.GetX(), y = southWest.GetY(); string(get((int)x, (int)y)) != ""; x -= s, y += c) {
		nodes.emplace_back(x, y);
		verticalNode1s.emplace_back(nodes.back(), (int)nodes.size() - 1);
	}
	for (float x = northEast.GetX(), y = northEast.GetY(); string(get((int)x, (int)y)) != ""; x += s, y -= c) {
		nodes.emplace_back(x, y);
		verticalNode2n.emplace_back(nodes.back(), (int)nodes.size() - 1);
	}
	for (float x = southEast.GetX(), y = southEast.GetY(); string(get((int)x, (int)y)) != ""; x -= s, y += c) {
		nodes.emplace_back(x, y);
		verticalNode2s.emplace_back(nodes.back(), (int)nodes.size() - 1);
	}

	connections.emplace_back("中山北路", nodes[0], nodes[1]);
	connections.emplace_back("中山东路", nodes[1], nodes[3]);
	connections.emplace_back("中山南路", nodes[3], nodes[2]);
	connections.emplace_back("中山西路", nodes[2], nodes[0]);

	for (size_t i = 1; i < horizontalNode1w.size(); i++) {
		const auto& [node1, idx1] = horizontalNode1w[i];
		const auto& [node2, idx2] = horizontalNode1w[i - 1];
		connections.emplace_back("城西北路", nodes[idx1], nodes[idx2]);
	}
	for (size_t i = 1; i < horizontalNode1e.size(); i++) {
		const auto& [node1, idx1] = horizontalNode1e[i];
		const auto& [node2, idx2] = horizontalNode1e[i - 1];
		connections.emplace_back("城东北路", nodes[idx1], nodes[idx2]);
	}
	for (size_t i = 1; i < horizontalNode2w.size(); i++) {
		const auto& [node1, idx1] = horizontalNode2w[i];
		const auto& [node2, idx2] = horizontalNode2w[i - 1];
		connections.emplace_back("城西南路", nodes[idx1], nodes[idx2]);
	}
	for (size_t i = 1; i < horizontalNode2e.size(); i++) {
		const auto& [node1, idx1] = horizontalNode2e[i];
		const auto& [node2, idx2] = horizontalNode2e[i - 1];
		connections.emplace_back("城东南路", nodes[idx1], nodes[idx2]);
	}
	for (size_t i = 1; i < verticalNode1n.size(); i++) {
		const auto& [node1, idx1] = verticalNode1n[i];
		const auto& [node2, idx2] = verticalNode1n[i - 1];
		connections.emplace_back("城北西路", nodes[idx1], nodes[idx2]);
	}
	for (size_t i = 1; i < verticalNode1s.size(); i++) {
		const auto& [node1, idx1] = verticalNode1s[i];
		const auto& [node2, idx2] = verticalNode1s[i - 1];
		connections.emplace_back("城南西路", nodes[idx1], nodes[idx2]);
	}
	for (size_t i = 1; i < verticalNode2n.size(); i++) {
		const auto& [node1, idx1] = verticalNode2n[i];
		const auto& [node2, idx2] = verticalNode2n[i - 1];
		connections.emplace_back("城北东路", nodes[idx1], nodes[idx2]);
	}
	for (size_t i = 1; i < verticalNode2s.size(); i++) {
		const auto& [node1, idx1] = verticalNode2s[i];
		const auto& [node2, idx2] = verticalNode2s[i - 1];
		connections.emplace_back("城南东路", nodes[idx1], nodes[idx2]);
	}

	lots.emplace_back(Lot(northWest, northEast, southEast, southWest, { 0.5f, 0.5f, 0.5f, 0.5f }), 
		vector<pair<Connection, float>>({
		{connections[0], 0.5f},
		{connections[1], 0.5f},
		{connections[2], 0.5f},
		{connections[3], 0.5f}
	}));
	lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);

	for (size_t i = 1; i < min(horizontalNode1w.size(), horizontalNode2w.size()); i++) {
		if (get((int)horizontalNode1w[i].first.GetX(), (int)horizontalNode1w[i].first.GetY()) != "plain") continue;
		if (get((int)horizontalNode1w[i - 1].first.GetX(), (int)horizontalNode1w[i - 1].first.GetY()) != "plain") continue;
		if (get((int)horizontalNode2w[i].first.GetX(), (int)horizontalNode2w[i].first.GetY()) != "plain") continue;
		if (get((int)horizontalNode2w[i - 1].first.GetX(), (int)horizontalNode2w[i - 1].first.GetY()) != "plain") continue;

		const auto& [nwNode, nwIdx] = horizontalNode1w[i];
		const auto& [neNode, neIdx] = horizontalNode1w[i - 1];
		const auto& [seNode, seIdx] = horizontalNode2w[i - 1];
		const auto& [swNode, swIdx] = horizontalNode2w[i];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.5f, (i == 1 ? 0.5f : 0.0f), 0.5f, 0.0f }),
			vector<pair<Connection, float>>({
			{Connection("城西北路", nodes[nwIdx], nodes[neIdx]), 0.5f},
			{Connection("城西南路", nodes[swIdx], nodes[seIdx]), 0.5f}
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(horizontalNode1e.size(), horizontalNode2e.size()); i++) {
		if (get((int)horizontalNode1e[i].first.GetX(), (int)horizontalNode1e[i].first.GetY()) != "plain") continue;
		if (get((int)horizontalNode1e[i - 1].first.GetX(), (int)horizontalNode1e[i - 1].first.GetY()) != "plain") continue;
		if (get((int)horizontalNode2e[i].first.GetX(), (int)horizontalNode2e[i].first.GetY()) != "plain") continue;
		if (get((int)horizontalNode2e[i - 1].first.GetX(), (int)horizontalNode2e[i - 1].first.GetY()) != "plain") continue;

		const auto& [nwNode, nwIdx] = horizontalNode1e[i - 1];
		const auto& [neNode, neIdx] = horizontalNode1e[i];
		const auto& [seNode, seIdx] = horizontalNode2e[i];
		const auto& [swNode, swIdx] = horizontalNode2e[i - 1];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.5f, 0.0f, 0.5f, (i == 1 ? 0.5f : 0.0f) }),
			vector<pair<Connection, float>>({
			{Connection("城东北路", nodes[neIdx], nodes[nwIdx]), 0.5f},
			{Connection("城东南路", nodes[seIdx], nodes[swIdx]), 0.5f}
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(verticalNode1n.size(), verticalNode2n.size()); i++) {
		if (get((int)verticalNode1n[i].first.GetX(), (int)verticalNode1n[i].first.GetY()) != "plain") continue;
		if (get((int)verticalNode1n[i - 1].first.GetX(), (int)verticalNode1n[i - 1].first.GetY()) != "plain") continue;
		if (get((int)verticalNode2n[i].first.GetX(), (int)verticalNode2n[i].first.GetY()) != "plain") continue;
		if (get((int)verticalNode2n[i - 1].first.GetX(), (int)verticalNode2n[i - 1].first.GetY()) != "plain") continue;

		const auto& [nwNode, nwIdx] = verticalNode1n[i];
		const auto& [neNode, neIdx] = verticalNode2n[i];
		const auto& [seNode, seIdx] = verticalNode2n[i - 1];
		const auto& [swNode, swIdx] = verticalNode1n[i - 1];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.0f, 0.5f, (i == 1 ? 0.5f : 0.0f), 0.5f }),
			vector<pair<Connection, float>>({
			{Connection("城北西路", nodes[nwIdx], nodes[swIdx]), 0.5f},
			{Connection("城北东路", nodes[neIdx], nodes[seIdx]), 0.5f}
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(verticalNode1s.size(), verticalNode2s.size()); i++) {
		if (get((int)verticalNode1s[i].first.GetX(), (int)verticalNode1s[i].first.GetY()) != "plain") continue;
		if (get((int)verticalNode1s[i - 1].first.GetX(), (int)verticalNode1s[i - 1].first.GetY()) != "plain") continue;
		if (get((int)verticalNode2s[i].first.GetX(), (int)verticalNode2s[i].first.GetY()) != "plain") continue;
		if (get((int)verticalNode2s[i - 1].first.GetX(), (int)verticalNode2s[i - 1].first.GetY()) != "plain") continue;

		const auto& [nwNode, nwIdx] = verticalNode1s[i - 1];
		const auto& [neNode, neIdx] = verticalNode2s[i - 1];
		const auto& [seNode, seIdx] = verticalNode2s[i];
		const auto& [swNode, swIdx] = verticalNode1s[i];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { (i == 1 ? 0.5f : 0.0f), 0.5f, 0.0f, 0.5f }),
			vector<pair<Connection, float>>({
			{Connection("城南西路", nodes[swIdx], nodes[nwIdx]), 0.5f},
			{Connection("城南东路", nodes[seIdx], nodes[neIdx]), 0.5f}
				}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
}

