#include "roadnet_basic.h"


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
	const function<string(int, int)>& get) {
	string meshPath = "/Game/Asset/Meshes/default_1_1.default_1_1";
		
	vector<pair<Node, int>> horizontalNode1w;
	vector<pair<Node, int>> horizontalNode1e;
	vector<pair<Node, int>> horizontalNode2w;
	vector<pair<Node, int>> horizontalNode2e;
	vector<pair<Node, int>> verticalNode1n;
	vector<pair<Node, int>> verticalNode1s;
	vector<pair<Node, int>> verticalNode2n;
	vector<pair<Node, int>> verticalNode2s;

	float theta = GetRandom(1000) / 1000.f * 0.4f - 0.2f;

	Intersection northWest(width / 2.f + 16.f * (sin(theta) - cos(theta)), height / 2.f + 16.f * (-cos(theta) - sin(theta)));
	Intersection northEast(width / 2.f + 16.f * (sin(theta) + cos(theta)), height / 2.f + 16.f * (-cos(theta) + sin(theta)));
	Intersection southWest(width / 2.f + 16.f * (-sin(theta) - cos(theta)), height / 2.f + 16.f * (cos(theta) - sin(theta)));
	Intersection southEast(width / 2.f + 16.f * (-sin(theta) + cos(theta)), height / 2.f + 16.f * (cos(theta) + sin(theta)));
	intersections.push_back(northWest);
	intersections.push_back(northEast);
	intersections.push_back(southWest);
	intersections.push_back(southEast);

	float dirSin = northEast.GetY() - northWest.GetY();
	float dirCos = northEast.GetX() - northWest.GetX();

	for (float x = northWest.GetX() - dirCos, y = northWest.GetY() - dirSin; string(get(static_cast<int>(x), static_cast<int>(y))) != ""; x -= dirCos, y -= dirSin) {
		intersections.emplace_back(x, y);
		horizontalNode1w.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
	}
	for (float x = northEast.GetX() + dirCos, y = northEast.GetY() + dirSin; string(get(static_cast<int>(x), static_cast<int>(y))) != ""; x += dirCos, y += dirSin) {
		intersections.emplace_back(x, y);
		horizontalNode1e.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
	}
	for (float x = southWest.GetX() - dirCos, y = southWest.GetY() - dirSin; string(get(static_cast<int>(x), static_cast<int>(y))) != ""; x -= dirCos, y -= dirSin) {
		intersections.emplace_back(x, y);
		horizontalNode2w.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
	}
	for (float x = southEast.GetX() + dirCos, y = southEast.GetY() + dirSin; string(get(static_cast<int>(x), static_cast<int>(y))) != ""; x += dirCos, y += dirSin) {
		intersections.emplace_back(x, y);
		horizontalNode2e.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
	}
	for (float x = northWest.GetX() + dirSin, y = northWest.GetY() - dirCos; string(get(static_cast<int>(x), static_cast<int>(y))) != ""; x += dirSin, y -= dirCos) {
		intersections.emplace_back(x, y);
		verticalNode1n.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
	}
	for (float x = southWest.GetX() - dirSin, y = southWest.GetY() + dirCos; string(get(static_cast<int>(x), static_cast<int>(y))) != ""; x -= dirSin, y += dirCos) {
		intersections.emplace_back(x, y);
		verticalNode1s.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
	}
	for (float x = northEast.GetX() + dirSin, y = northEast.GetY() - dirCos; string(get(static_cast<int>(x), static_cast<int>(y))) != ""; x += dirSin, y -= dirCos) {
		intersections.emplace_back(x, y);
		verticalNode2n.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
	}
	for (float x = southEast.GetX() - dirSin, y = southEast.GetY() + dirCos; string(get(static_cast<int>(x), static_cast<int>(y))) != ""; x -= dirSin, y += dirCos) {
		intersections.emplace_back(x, y);
		verticalNode2s.emplace_back(intersections.back(), static_cast<int>(intersections.size()) - 1);
	}

	roads.emplace_back("中山北路", intersections[0], intersections[1], meshPath);
	roads.emplace_back("中山东路", intersections[1], intersections[3], meshPath);
	roads.emplace_back("中山南路", intersections[3], intersections[2], meshPath);
	roads.emplace_back("中山西路", intersections[2], intersections[0], meshPath);

	if (horizontalNode1w.size() > 0) {
		roads.emplace_back("城西北路", intersections[0], intersections[horizontalNode1w[0].second], meshPath);
	}
	for (size_t i = 1; i < horizontalNode1w.size(); i++) {
		const auto& [node1, idx1] = horizontalNode1w[i];
		const auto& [node2, idx2] = horizontalNode1w[i - 1];
		roads.emplace_back("城西北路", intersections[idx1], intersections[idx2], meshPath);
	}
	if (horizontalNode1e.size() > 0) {
		roads.emplace_back("城东北路", intersections[1], intersections[horizontalNode1e[0].second], meshPath);
	}
	for (size_t i = 1; i < horizontalNode1e.size(); i++) {
		const auto& [node1, idx1] = horizontalNode1e[i];
		const auto& [node2, idx2] = horizontalNode1e[i - 1];
		roads.emplace_back("城东北路", intersections[idx1], intersections[idx2], meshPath);
	}
	if (horizontalNode2w.size() > 0) {
		roads.emplace_back("城西南路", intersections[2], intersections[horizontalNode2w[0].second], meshPath);
	}
	for (size_t i = 1; i < horizontalNode2w.size(); i++) {
		const auto& [node1, idx1] = horizontalNode2w[i];
		const auto& [node2, idx2] = horizontalNode2w[i - 1];
		roads.emplace_back("城西南路", intersections[idx1], intersections[idx2], meshPath);
	}
	if (horizontalNode2e.size() > 0) {
		roads.emplace_back("城东南路", intersections[3], intersections[horizontalNode2e[0].second], meshPath);
	}
	for (size_t i = 1; i < horizontalNode2e.size(); i++) {
		const auto& [node1, idx1] = horizontalNode2e[i];
		const auto& [node2, idx2] = horizontalNode2e[i - 1];
		roads.emplace_back("城东南路", intersections[idx1], intersections[idx2], meshPath);
	}
	if (verticalNode1n.size() > 0) {
		roads.emplace_back("城北西路", intersections[0], intersections[verticalNode1n[0].second], meshPath);
	}
	for (size_t i = 1; i < verticalNode1n.size(); i++) {
		const auto& [node1, idx1] = verticalNode1n[i];
		const auto& [node2, idx2] = verticalNode1n[i - 1];
		roads.emplace_back("城北西路", intersections[idx1], intersections[idx2], meshPath);
	}
	if (verticalNode1s.size() > 0) {
		roads.emplace_back("城南西路", intersections[2], intersections[verticalNode1s[0].second], meshPath);
	}
	for (size_t i = 1; i < verticalNode1s.size(); i++) {
		const auto& [node1, idx1] = verticalNode1s[i];
		const auto& [node2, idx2] = verticalNode1s[i - 1];
		roads.emplace_back("城南西路", intersections[idx1], intersections[idx2], meshPath);
	}
	if (verticalNode2n.size() > 0) {
		roads.emplace_back("城北东路", intersections[1], intersections[verticalNode2n[0].second], meshPath);
	}
	for (size_t i = 1; i < verticalNode2n.size(); i++) {
		const auto& [node1, idx1] = verticalNode2n[i];
		const auto& [node2, idx2] = verticalNode2n[i - 1];
		roads.emplace_back("城北东路", intersections[idx1], intersections[idx2], meshPath);
	}
	if (verticalNode2s.size() > 0) {
		roads.emplace_back("城南东路", intersections[3], intersections[verticalNode2s[0].second], meshPath);
	}
	for (size_t i = 1; i < verticalNode2s.size(); i++) {
		const auto& [node1, idx1] = verticalNode2s[i];
		const auto& [node2, idx2] = verticalNode2s[i - 1];
		roads.emplace_back("城南东路", intersections[idx1], intersections[idx2], meshPath);
	}

	lots.emplace_back(Lot(northWest, northEast, southEast, southWest, { 0.5f, 0.5f, 0.5f, 0.5f }),
		vector<pair<Road, float>>({
		{roads[0], 0.5f},
		{roads[1], 0.5f},
		{roads[2], 0.5f},
		{roads[3], 0.5f}
	}));
	lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	return;

	if (horizontalNode1w.size() >= 1 && horizontalNode2w.size() >= 1) {
		lots.emplace_back(Lot(horizontalNode1w[0].first, northWest, southWest, horizontalNode2w[0].first, { 0.5f, 0.5f, 0.5f, 0.0f }),
			vector<pair<Road, float>>({
					{Road("城西北路", intersections[horizontalNode1w[0].second], intersections[0], meshPath), 0.5f},
					{roads[3], 0.5f},
					{Road("城西南路", intersections[horizontalNode2w[0].second], intersections[2], meshPath), 0.5f},
				}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
	if (horizontalNode1e.size() >= 1 && horizontalNode2e.size() >= 1) {
		lots.emplace_back(Lot(northEast, horizontalNode1e[0].first, horizontalNode2e[0].first, southEast, { 0.5f, 0.0f, 0.5f, 0.5f }),
			vector<pair<Road, float>>({
					{Road("城东北路", intersections[horizontalNode1e[0].second], intersections[1], meshPath), 0.5f},
					{roads[1], 0.5f},
					{Road("城东南路", intersections[horizontalNode2e[0].second], intersections[3], meshPath), 0.5f},
				}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
	if (verticalNode1n.size() >= 1 && verticalNode2n.size() >= 1) {
		lots.emplace_back(Lot(verticalNode1n[0].first, verticalNode2n[0].first, northEast, northWest, { 0.5f, 0.5f, 0.0f, 0.5f }),
			vector<pair<Road, float>>({
					{Road("城北西路", intersections[verticalNode1n[0].second], intersections[0], meshPath), 0.5f},
					{roads[1], 0.5f},
					{Road("城北东路", intersections[verticalNode2n[0].second], intersections[1], meshPath), 0.5f},
				}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
	if (verticalNode1s.size() >= 1 && verticalNode2s.size() >= 1) {
		lots.emplace_back(Lot(southEast, southWest, verticalNode2s[0].first, verticalNode1s[0].first, { 0.0f, 0.5f, 0.5f, 0.5f }),
			vector<pair<Road, float>>({
					{Road("城南西路", intersections[verticalNode1s[0].second], intersections[2], meshPath), 0.5f},
					{roads[1], 0.5f},
					{Road("城南东路", intersections[verticalNode2s[0].second], intersections[3], meshPath), 0.5f},
				}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
	return;

	for (size_t i = 1; i < min(horizontalNode1w.size(), horizontalNode2w.size()); i++) {
		if (get(static_cast<int>(horizontalNode1w[i].first.GetX()), static_cast<int>(horizontalNode1w[i].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(horizontalNode1w[i - 1].first.GetX()), static_cast<int>(horizontalNode1w[i - 1].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(horizontalNode2w[i].first.GetX()), static_cast<int>(horizontalNode2w[i].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(horizontalNode2w[i - 1].first.GetX()), static_cast<int>(horizontalNode2w[i - 1].first.GetY())) != "construction") continue;

		const auto& [nwNode, nwIdx] = horizontalNode1w[i];
		const auto& [neNode, neIdx] = horizontalNode1w[i - 1];
		const auto& [seNode, seIdx] = horizontalNode2w[i - 1];
		const auto& [swNode, swIdx] = horizontalNode2w[i];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.5f, 0.0f, 0.5f, 0.0f }),
			vector<pair<Road, float>>({
			{Road("城西北路", intersections[nwIdx], intersections[neIdx], meshPath), 0.5f},
			{Road("城西南路", intersections[swIdx], intersections[seIdx], meshPath), 0.5f}
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(horizontalNode1e.size(), horizontalNode2e.size()); i++) {
		if (get(static_cast<int>(horizontalNode1e[i].first.GetX()), static_cast<int>(horizontalNode1e[i].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(horizontalNode1e[i - 1].first.GetX()), static_cast<int>(horizontalNode1e[i - 1].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(horizontalNode2e[i].first.GetX()), static_cast<int>(horizontalNode2e[i].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(horizontalNode2e[i - 1].first.GetX()), static_cast<int>(horizontalNode2e[i - 1].first.GetY())) != "construction") continue;

		const auto& [nwNode, nwIdx] = horizontalNode1e[i - 1];
		const auto& [neNode, neIdx] = horizontalNode1e[i];
		const auto& [seNode, seIdx] = horizontalNode2e[i];
		const auto& [swNode, swIdx] = horizontalNode2e[i - 1];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.5f, 0.0f, 0.5f, 0.0f }),
			vector<pair<Road, float>>({
			{Road("城东北路", intersections[neIdx], intersections[nwIdx], meshPath), 0.5f},
			{Road("城东南路", intersections[seIdx], intersections[swIdx], meshPath), 0.5f}
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(verticalNode1n.size(), verticalNode2n.size()); i++) {
		if (get(static_cast<int>(verticalNode1n[i].first.GetX()), static_cast<int>(verticalNode1n[i].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(verticalNode1n[i - 1].first.GetX()), static_cast<int>(verticalNode1n[i - 1].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(verticalNode2n[i].first.GetX()), static_cast<int>(verticalNode2n[i].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(verticalNode2n[i - 1].first.GetX()), static_cast<int>(verticalNode2n[i - 1].first.GetY())) != "construction") continue;

		const auto& [nwNode, nwIdx] = verticalNode1n[i];
		const auto& [neNode, neIdx] = verticalNode2n[i];
		const auto& [seNode, seIdx] = verticalNode2n[i - 1];
		const auto& [swNode, swIdx] = verticalNode1n[i - 1];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.0f, 0.5f, 0.0f, 0.5f }),
			vector<pair<Road, float>>({
			{Road("城北西路", intersections[nwIdx], intersections[swIdx], meshPath), 0.5f},
			{Road("城北东路", intersections[neIdx], intersections[seIdx], meshPath), 0.5f}
		}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}

	for (size_t i = 1; i < min(verticalNode1s.size(), verticalNode2s.size()); i++) {
		if (get(static_cast<int>(verticalNode1s[i].first.GetX()), static_cast<int>(verticalNode1s[i].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(verticalNode1s[i - 1].first.GetX()), static_cast<int>(verticalNode1s[i - 1].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(verticalNode2s[i].first.GetX()), static_cast<int>(verticalNode2s[i].first.GetY())) != "construction") continue;
		if (get(static_cast<int>(verticalNode2s[i - 1].first.GetX()), static_cast<int>(verticalNode2s[i - 1].first.GetY())) != "construction") continue;

		const auto& [nwNode, nwIdx] = verticalNode1s[i - 1];
		const auto& [neNode, neIdx] = verticalNode2s[i - 1];
		const auto& [seNode, seIdx] = verticalNode2s[i];
		const auto& [swNode, swIdx] = verticalNode1s[i];

		lots.emplace_back(Lot(nwNode, neNode, seNode, swNode, { 0.0f, 0.5f, 0.0f, 0.5f }),
			vector<pair<Road, float>>({
			{Road("城南西路", intersections[swIdx], intersections[nwIdx], meshPath), 0.5f},
			{Road("城南东路", intersections[seIdx], intersections[neIdx], meshPath), 0.5f}
				}));
		lots.back().first.SetArea(AREA_RESIDENTIAL_LOW);
	}
}
