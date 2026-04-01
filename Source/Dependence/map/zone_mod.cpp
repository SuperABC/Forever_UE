#include "zone_mod.h"


using namespace std;

ZoneMod::ZoneMod() :
	acreage(0.f),
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
	for (auto& config : configs) {
		config.second = false;
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

	int num = assigner->second(lot);
	for (int i = 0; i < num; ++i) {
		ZoneMod* zoneMod = CreateZone(type);
		if (zoneMod) {
			zoneMods.push_back(zoneMod);
		}
	}
	return zoneMods;
}

void ZoneFactory::DestroyZone(ZoneMod* zone) const {
	if (!zone) {
		debugf("Warning: Zone is null when deleting.\n");
		return;
	}

	auto it = registries.find(zone->GetType());
	if (it == registries.end()) {
		debugf("Warning: Zone %s not registered when deleting.\n", zone->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(zone);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Zone " + string(zone->GetType()) + " deleter is null.\n");
	}
}

