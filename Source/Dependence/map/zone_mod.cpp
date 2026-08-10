#include "zone_mod.h"


using namespace std;

ZoneMod::ZoneMod() :
	acreage(0.f),
	mesh("/Game/Asset/Meshes/Cube.Cube"),
	unit(0.1f),
	buildings(),
	script({ "empty", {} }) {

}

ZoneMod::~ZoneMod() {

}

void ZoneFactory::RegisterZone(const string& id, function<int(const Lot*)> assigner,
	function<ZoneMod* ()> creator, function<void(ZoneMod*)> deleter) {
	registries[id] = { creator, deleter };
	assigners[id] = assigner;
}

void ZoneFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

ZoneMod* ZoneFactory::CreateZone(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Zone %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Zone %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Zone " + id + " creater is null.\n");
	}

	return nullptr;
}

bool ZoneFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void ZoneFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

vector<string> ZoneFactory::GetTypes() const {
	vector<string> types;
	for (const auto& [name, enabled] : configs) {
		if (enabled) types.push_back(name);
	}
	return types;
}

vector<ZoneMod*> ZoneFactory::CreateZones(const string& type, const Lot* lot) const {
	vector<ZoneMod*> zoneMods;
	auto config = configs.find(type);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Zone %s not enabled when creating.\n", type.data());
		return zoneMods;
	}

	auto assigner = assigners.find(type);
	if (assigner == assigners.end()) {
		debugf("Warning: Generator for zone %s not found when creating.\n", type.data());
		return zoneMods;
	}

	int zoneCount = assigner->second(lot);
	for (int i = 0; i < zoneCount; i++) {
		ZoneMod* zoneMod = CreateZone(type);
		if (zoneMod) {
			zoneMods.push_back(zoneMod);
		}
	}
	return zoneMods;
}

void ZoneFactory::DestroyZone(ZoneMod* zoneMod) const {
	if (!zoneMod) {
		debugf("Warning: Zone is null when deleting.\n");
		return;
	}

	auto it = registries.find(zoneMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Zone %s not registered when deleting.\n", zoneMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(zoneMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Zone " + string(zoneMod->GetType()) + " deleter is null.\n");
	}
}

