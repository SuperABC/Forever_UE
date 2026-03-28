#include "building_mod.h"


using namespace std;

BuildingMod::BuildingMod() :
	maxAcreage(10000.f),
	minAcreage(0.f),
	layers(1),
	basements(0),
	height(0.4f),
	construction(),
	templates(),
	singles(),
	rows(),
	script({ "empty", "" }) {

}

BuildingMod::~BuildingMod() {

}

void BuildingMod::AllocateFloors() {
	templates.resize(basements + layers);
}

void BuildingMod::AssignFloor(int level, int face, string layout) {
	templates.resize(basements + layers);
	level += basements;
	if (level < 0 || level >= templates.size()) {
		THROW_EXCEPTION(OutOfRangeException, "Assigning floor our of basements or layers range.\n");
	}
	templates[level] = { layout, face };
}

void BuildingMod::AssignFloors(int face, string layout) {
	templates.resize(basements + layers);
	for (int i = 0; i < basements + layers; i++) {
		templates[i] = { layout, face };
	}
}

void BuildingMod::AssignFloors(int face, vector<string> layouts) {
	if (layouts.size() != basements + layers) {
		THROW_EXCEPTION(RuntimeException, "Building layout numbers mismatch the floor numbers.\n");
	}
	templates.resize(basements + layers);
	for (int i = 0; i < basements + layers; i++) {
		templates[i] = { layouts[i], face};
	}
}

void BuildingMod::AssignRoom(int level, int slot, string room, string component, int id) {
	level += basements;
	singles[{ component, id }].emplace_back(level, slot, room);
}

void BuildingMod::ArrangeRow(int level, int slot, string room, float acreage, string component, int id) {
	level += basements;
	if (rows.find({ component, id }) == rows.end()) {
		rows[{ component, id }].resize(basements + layers);
	}
	rows[{ component, id }][level].emplace_back(slot, room, acreage);
}

void BuildingFactory::RegisterBuilding(const string& id,
	const vector<float>& power, function<int(Lot*)> assigner,
	function<BuildingMod* ()> creator, function<void(BuildingMod*)> deleter) {
	registries[id] = { creator, deleter };
	powers[id] = power;
	assigners[id] = assigner;
}

void BuildingFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

BuildingMod* BuildingFactory::CreateBuilding(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Building %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Building %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Building " + id + " creater is null.\n");
	}

	return nullptr;
}

bool BuildingFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void BuildingFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

unordered_map<string, vector<float>> BuildingFactory::GetPowers() const {
	auto result = powers;
	for (auto& [name, power] : powers) {
		if (configs.find(name) == configs.end() || !configs.at(name)) {
			result.erase(name);
		}
	}
	return result;
}

vector<string> BuildingFactory::CreateBuildings(const string& type, Lot* lot) const {
	vector<string> buildings;
	auto config = configs.find(type);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Zone %s not enabled when creating.\n", type.data());
		return buildings;
	}

	auto assigner = assigners.find(type);
	if (assigner == assigners.end()) {
		debugf("Warning: Generator for zone %s not found when creating.\n", type.data());
		return buildings;
	}

	int num = assigner->second(lot);
	for (int i = 0; i < num; ++i) {
		buildings.push_back(type);
	}
	return buildings;
}

void BuildingFactory::DestroyBuilding(BuildingMod* building) const {
	if (!building) {
		debugf("Warning: Building is null when deleting.\n");
		return;
	}

	auto it = registries.find(building->GetType());
	if (it == registries.end()) {
		debugf("Warning: Building %s not registered when deleting.\n", building->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(building);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Building " + string(building->GetType()) + " deleter is null.\n");
	}
}
