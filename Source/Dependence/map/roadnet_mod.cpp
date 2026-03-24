#include "roadnet_mod.h"


using namespace std;

RoadnetMod::RoadnetMod() :
	nodes(),
	connections(),
	lots() {

}

RoadnetMod::~RoadnetMod() {

}

void RoadnetFactory::RegisterRoadnet(const string& id,
	function<RoadnetMod* ()> creator, function<void(RoadnetMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void RoadnetFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
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

void RoadnetFactory::DestroyRoadnet(RoadnetMod* roadnet) const {
	if (!roadnet) {
		debugf("Warning: Roadnet is null when deleting.\n");
		return;
	}

	auto it = registries.find(roadnet->GetType());
	if (it == registries.end()) {
		debugf("Warning: Roadnet %s not registered when deleting.\n", roadnet->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(roadnet);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Roadnet " + string(roadnet->GetType()) + " deleter is null.\n");
	}
}

