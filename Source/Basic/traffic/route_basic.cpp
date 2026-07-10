#include "route_basic.h"

#include <cmath>

using namespace std;

int AirRoute::count = 0;

AirRoute::AirRoute() : id(count++) {

}

AirRoute::~AirRoute() {

}

const char* AirRoute::GetId() {
	return "air";
}

const char* AirRoute::GetType() const {
	return "air";
}

const char* AirRoute::GetName() {
	name = "飞机线路" + to_string(id);
	return name.data();
}

void AirRoute::LayoutRoute(
	const unordered_map<string, vector<pair<vector<Node*>, vector<pair<Node*, Node*>>>>>& interfaces,
	int sizeX, int sizeY, int nodeStaticCount) {
	Node::SetCount(nodeStaticCount);

	auto airports = interfaces.find("air")->second;
	for (auto& [pivots, rays] : airports) {
		if (pivots.size() != 1)continue;
		for(auto& ray : rays) {
			nodes.push_back(Node(*pivots[0]));
			nodes.push_back(Node(*ray.first));
			connections.push_back(Connection(*pivots[0], *ray.first));

			float x0 = ray.first->GetX(), y0 = ray.first->GetY();
			float dx = ray.second->GetX() - x0, dy = ray.second->GetY() - y0;
			float len = sqrtf(dx * dx + dy * dy);
			if (len < 1e-6f) continue;
			float ndx = dx / len, ndy = dy / len;

			float t = -1.f;
			auto tryT = [&](float candidate) {
				if (candidate > 0.f && (t < 0.f || candidate < t)) t = candidate;
			};
			if (ndx > 1e-6f) tryT((float(sizeX) - x0) / ndx);
			else if (ndx < -1e-6f) tryT(-x0 / ndx);
			if (ndy > 1e-6f) tryT((float(sizeY) - y0) / ndy);
			else if (ndy < -1e-6f) tryT(-y0 / ndy);
			if (t < 0.f) continue;

			float bx = x0 + t * ndx, by = y0 + t * ndy;
			constexpr float bz = 100.f;

			Node externNode("air", bx, by, bz);
			externs.push_back(externNode);

			Connection airConn(*ray.first, externNode);
			airConn.AddControls({
				{Node("", x0 + (bx - x0) / 3.f, y0 + (by - y0) / 3.f, 0.f), 2.0f},
				{Node("", x0 + 2.f * (bx - x0) / 3.f, y0 + 2.f * (by - y0) / 3.f, bz), 2.0f}
			});
			connections.push_back(airConn);
		}
	}
}
