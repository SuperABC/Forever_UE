#include "terrain_mod.h"


using namespace std;

TerrainMod::TerrainMod() :
	dx{ -1, 1, 0, 0 },
	dy{ 0, 0, -1, 1 } {

}

TerrainMod::~TerrainMod() {

}

int TerrainMod::FloodTerrain(
	int x, int y, int num, bool overwrite, int width, int height,
	function<string(int, int)> get, function<bool(int, int, string)> set) const {
	vector<pair<int, int>> q;

	if (x < 0 || x >= width - 1 || y < 0 || y >= height - 1)
		return 0;
	if (get(x, y) == GetType())
		return 0;

	set(x, y, GetType());
	int count = 1;

	for (int i = 0; i < 4; ++i) {
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
			q.push_back(make_pair(nx, ny));
		}
	}

	while (count < num && !q.empty()) {
		int idx = GetRandom((int)q.size());
		auto [cx, cy] = q[idx];
		q[idx] = q.back();
		q.pop_back();

		set(cx, cy, GetType());
		++count;

		UpdateBoundary(cx, cy, q, overwrite, width, height, get, set);
	}

	return count;
}

bool TerrainMod::CheckBoundary(
	int x, int y, bool overwrite, int width, int height,
	function<string(int, int)> get, function<bool(int, int, string)> set) const {
	if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
		return true;
	for (int i = 0; i < 4; ++i) {
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (overwrite) {
			if (get(nx, ny) != GetType())
				return true;
		}
		else {
			if (get(nx, ny) == "plain")
				return true;
		}
	}
	return false;
}

void TerrainMod::UpdateBoundary(
	int x, int y, vector<pair<int, int>>& q, bool overwrite, int width, int height,
	function<string(int, int)> get, function<bool(int, int, string)> set) const {
	for (int i = 0; i < 4; ++i) {
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
			CheckBoundary(nx, ny, overwrite, width, height, get, set)) {
			bool overlap = false;
			for (auto [bx, by] : q) {
				if (bx == nx && by == ny) {
					overlap = true;
					break;
				}
			}
			if (!overlap)
				q.push_back(make_pair(nx, ny));
		}
	}
}

void TerrainMod::ShapeFilter(int x, int y, int width, int height,
	function<string(int, int)> get, function<bool(int, int, string)> set,
	int side, float threshold) const {
	if (get(x, y) == GetType())
		return;

	int count = 0;
	int total = (side * 2 + 1) * (side * 2 + 1);
	int x_start = max(0, x - side);
	int x_end = min(width - 1, x + side);
	int y_start = max(0, y - side);
	int y_end = min(height - 1, y + side);

	for (int i = x_start; i <= x_end; ++i) {
		for (int j = y_start; j <= y_end; ++j) {
			if (get(i, j) == GetType())
				++count;
		}
	}
	
	int actual_total = (x_end - x_start + 1) * (y_end - y_start + 1);
	if (count > actual_total * threshold)
		set(x, y, GetType());
}

void TerrainFactory::RegisterTerrain(const string& id,
	function<TerrainMod* ()> creator, function<void(TerrainMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void TerrainFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
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

void TerrainFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

vector<string> TerrainFactory::GetTerrains() const {
	vector<string> terrains;
	for (auto& registry : registries) {
		auto config = configs.find(registry.first);
		if (config != configs.end() && config->second) {
			terrains.push_back(registry.first);
		}
	}
	return terrains;
}

void TerrainFactory::DestroyTerrain(TerrainMod* terrain) const {
	if (!terrain) {
		debugf("Warning: Terrain is null when deleting.\n");
		return;
	}

	auto it = registries.find(terrain->GetType());
	if (it == registries.end()) {
		debugf("Warning: Terrain %s not registered when deleting.\n", terrain->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(terrain);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Terrain " + string(terrain->GetType()) + " deleter is null.\n");
	}
}

