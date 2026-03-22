#include "building_mod.h"


using namespace std;

// 无构造
BuildingMod::BuildingMod() {

}

// 无析构
BuildingMod::~BuildingMod() {

}

// 根据楼层分配空间
void BuildingMod::AllocateFloors() {
	templates.resize(basements + layers);
}

// 根据模板生成一层楼层
void BuildingMod::AssignFloor(int level, int face, string layout) {
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

void BuildingMod::AssignRoom(int level, int slot, string name, string component, int id) {
	singles[{ component, id }].emplace_back(level, slot);
}

void BuildingMod::ArrangeRow(int level, int slot, string name, float acreage, string component, int id) {
	if (rows.find({ component, id }) == rows.end()) {
		rows[{ component, id }].resize(basements + layers);
	}
	rows[{ component, id }][level].emplace_back(slot, acreage);
}

// 注册建筑
void BuildingFactory::RegisterBuilding(const string& id,
	const vector<float>& power, function<int(Lot*)> assigner,
	function<BuildingMod* ()> creator, function<void(BuildingMod*)> deleter) {
	registries[id] = { creator, deleter };
	powers[id] = power;
	assigners[id] = assigner;
}

// 清空注册
void BuildingFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

// 创建建筑
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

// 检查是否注册
bool BuildingFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

// 设置启用配置
void BuildingFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

// 获取所有建筑全地块权重
unordered_map<string, vector<float>> BuildingFactory::GetPowers() const {
	auto result = powers;
	for (auto& [name, power] : powers) {
		if (configs.find(name) == configs.end() || !configs.at(name)) {
			result.erase(name);
		}
	}
	return result;
}

// 在地块内生成一类建筑
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

// 析构建筑
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
