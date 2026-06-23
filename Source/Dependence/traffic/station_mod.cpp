#include "station_mod.h"


using namespace std;

StationMod::StationMod() {

}

StationMod::~StationMod() {

}

void StationFactory::RegisterStation(const string& id,
	function<StationMod* ()> creator, function<void(StationMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void StationFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

StationMod* StationFactory::CreateStation(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Station %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Station %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Station " + id + " creater is null.\n");
	}

	return nullptr;
}

bool StationFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void StationFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void StationFactory::DestroyStation(StationMod* stationMod) const {
	if (!stationMod) {
		debugf("Warning: Station is null when deleting.\n");
		return;
	}

	auto it = registries.find(stationMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Station %s not registered when deleting.\n", stationMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(stationMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Station " + string(stationMod->GetType()) + " deleter is null.\n");
	}
}

