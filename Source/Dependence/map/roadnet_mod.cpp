#include "roadnet_mod.h"

#include <cmath>


using namespace std;

RoadnetMod::RoadnetMod() :
	intersections(),
	roads(),
	lots(),
	hatches() {

}

RoadnetMod::~RoadnetMod() {

}

void RoadnetMod::AddHatch(Connection* connection, float t1, float t2, float width) {
	if (!connection) {
		debugf("Warning: AddHatch connection is null.\n");
		return;
	}

	Node p1 = connection->GetPoint(t1);
	Node p2 = connection->GetPoint(t2);
	float dx = p2.GetX() - p1.GetX();
	float dy = p2.GetY() - p1.GetY();
	float length = sqrt(dx * dx + dy * dy);
	float rotation = atan2(dy, dx);
	float cx = (p1.GetX() + p2.GetX()) * 0.5f;
	float cy = (p1.GetY() + p2.GetY()) * 0.5f;
	hatches.emplace_back(Quad(cx, cy, length, width), rotation);
}

void RoadnetFactory::RegisterRoadnet(const string& id,
	function<RoadnetMod* ()> creator, function<void(RoadnetMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void RoadnetFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

RoadnetMod* RoadnetFactory::CreateRoadnet(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Roadnet %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Roadnet %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Roadnet " + id + " creator is null.\n");
	}

	return nullptr;
}

bool RoadnetFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void RoadnetFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

string RoadnetFactory::GetRoadnet() const {
	for (auto [name, enabled] : configs) {
		if (enabled) {
			return name;
		}
	}
	return "";
}

void RoadnetFactory::DestroyRoadnet(RoadnetMod* roadnetMod) const {
	if (!roadnetMod) {
		debugf("Warning: Roadnet is null when deleting.\n");
		return;
	}

	auto it = registries.find(roadnetMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Roadnet %s not registered when deleting.\n", roadnetMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(roadnetMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Roadnet " + string(roadnetMod->GetType()) + " deleter is null.\n");
	}
}

