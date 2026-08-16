#include "terrain_mod.h"


using namespace std;

TerrainMod::TerrainMod() :
	diffusePath(),
	waterHeight({ false, 0.f }) {

}

TerrainMod::~TerrainMod() {

}

void TerrainMod::ShapeFilter(int x, int y, int width, int height,
	function<string(int, int)> get, function<bool(int, int, string)> set,
	int side, float threshold) const {
	// 只有当前element不是当前地形时才计算滤波
	if (get(x, y) == GetType())
		return;

	// 计算邻域内当前地形element数量
	int count = 0;
	int total = (side * 2 + 1) * (side * 2 + 1);
	int xStart = max(0, x - side);
	int xEnd = min(width - 1, x + side);
	int yStart = max(0, y - side);
	int yEnd = min(height - 1, y + side);
	for (int i = xStart; i <= xEnd; i++) {
		for (int j = yStart; j <= yEnd; j++) {
			if (get(i, j) == GetType())
				++count;
		}
	}

	// 邻域内当前地形占比超过阈值时才将该点同化
	int actualTotal = (xEnd - xStart + 1) * (yEnd - yStart + 1);
	if (count > actualTotal * threshold)
		set(x, y, GetType());
}

TerrainFactory::TerrainFactory()
	: registries(),
	configs(),
	temp() {
	temp.registries.reserve(TEMP_RESERVE_CAPACITY);
}

void TerrainFactory::RegisterTerrain(const string& id,
	function<TerrainMod* ()> creator, function<void(TerrainMod*)> deleter) {
	temp.registries[id] = { creator, deleter };
}

void TerrainFactory::MergeTemp() {
	for (auto& [id, registry] : temp.registries) {
		registries[id] = registry;
	}
}

void TerrainFactory::CleanTemp() {
	temp.registries.clear();
}

void TerrainFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

TerrainMod* TerrainFactory::CreateTerrain(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Terrain %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Terrain %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Terrain " + id + " creater is null.\n");
	}

	return nullptr;
}

bool TerrainFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void TerrainFactory::SetConfig(const string& id, bool config) {
	configs[id] = config;
}

vector<string> TerrainFactory::GetTerrains() const {
	vector<string> terrains;
	for (auto& [id, _] : registries) {
		auto config = configs.find(id);
		if (config != configs.end() && config->second) {
			terrains.push_back(id);
		}
	}
	return terrains;
}

void TerrainFactory::DestroyTerrain(TerrainMod* terrainMod) const {
	if (!terrainMod) {
		debugf("Warning: Terrain is null when deleting.\n");
		return;
	}

	auto it = registries.find(terrainMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Terrain %s not registered when deleting.\n", terrainMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(terrainMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Terrain " + string(terrainMod->GetType()) + " deleter is null.\n");
	}
}

