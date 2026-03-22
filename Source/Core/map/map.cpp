#include "map.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <algorithm>


using namespace std;

TerrainFactory* Map::terrainFactory = nullptr;
RoadnetFactory* Map::roadnetFactory = nullptr;
ZoneFactory* Map::zoneFactory = nullptr;
BuildingFactory* Map::buildingFactory = nullptr;

Element::Element() :
	terrain("plain"),
	height(0.f),
	zone(),
	building() {

}

Element::~Element() {

}

string Element::GetTerrain() const {
	return terrain;
}

bool Element::SetTerrain(const string& terrain, float height) {
	this->terrain = terrain;
	this->height = height;
	return true;
}

float Element::GetHeight() const {
	return height;
}

bool Element::SetHeight(float height) {
	this->height = height;
	return true;
}

string Element::GetZone() const {
	return zone;
}

bool Element::SetZone(const string& zone) {
	this->zone = zone;
	return true;
}

string Element::GetBuilding() const {
	return building;
}

bool Element::SetBuilding(const string& building) {
	this->building = building;
	return true;
}

Chunk::Chunk(int x, int y) : offsetX(x), offsetY(y) {
	elements = vector<vector<shared_ptr<Element>>>(CHUNK_SIZE,
		vector<shared_ptr<Element>>(CHUNK_SIZE, nullptr));
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			elements[j][i] = make_shared<Element>();
		}
	}
}

Chunk::~Chunk() {

}

string Chunk::GetTerrain(int x, int y) const {
	if (!CheckXY(x, y)) {
		return "";
	}
	return elements[y - offsetY][x - offsetX]->GetTerrain();
}

bool Chunk::SetTerrain(int x, int y, const string& terrain, float height) {
	if (!CheckXY(x, y)) {
		return false;
	}
	return elements[y - offsetY][x - offsetX]->SetTerrain(terrain, height);
}

float Chunk::GetHeight(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid chunk coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return 0.f;
	}
	return elements[y - offsetY][x - offsetX]->GetHeight();
}

bool Chunk::CheckXY(int x, int y) const {
	if (x < offsetX) return false;
	if (y < offsetY) return false;
	if (x >= offsetX + CHUNK_SIZE) return false;
	if (y >= offsetY + CHUNK_SIZE) return false;
	return true;
}

shared_ptr<Element> Chunk::GetElement(int x, int y) const {
	if (!CheckXY(x, y)) {
		return nullptr;
	}
	return elements[y - offsetY][x - offsetX];
}

Map::Map() :
	width(0),
	height(0) {
	if (!terrainFactory) {
		terrainFactory = new TerrainFactory();
	}
	if (!roadnetFactory) {
		roadnetFactory = new RoadnetFactory();
	}
	if (!zoneFactory) {
		zoneFactory = new ZoneFactory();
	}
	if (!buildingFactory) {
		buildingFactory = new BuildingFactory();
	}
	//if (!componentFactory) {
	//	componentFactory = new ComponentFactory();
	//}
	//if (!roomFactory) {
	//	roomFactory = new RoomFactory();
	//}
}

Map::~Map() {
	delete roadnet;
}

void Map::LoadConfigs() const {
	terrainFactory->RemoveAll();
	roadnetFactory->RemoveAll();
	zoneFactory->RemoveAll();
	buildingFactory->RemoveAll();
	//componentFactory->RemoveAll();
	//roomFactory->RemoveAll();

	auto terrains = Config::GetEnables("terrain");
	for (auto terrain : terrains) {
		terrainFactory->SetConfig(terrain, true);
	}
	auto roadnets = Config::GetEnables("roadnet");
	if (roadnets.size() != 1) {
		THROW_EXCEPTION(RuntimeException, "There should be one and only one enabled roadnet.\n");
	}
	roadnetFactory->SetConfig(roadnets[0], true);
	auto zones = Config::GetEnables("zone");
	for (auto zone : zones) {
		zoneFactory->SetConfig(zone, true);
	}
	auto buildings = Config::GetEnables("building");
	for (auto building : buildings) {
		buildingFactory->SetConfig(building, true);
	}
	//auto components = Config::GetEnables("component");
	//for (auto component : components) {
	//	componentFactory->SetConfig(component, true);
	//}
	//auto rooms = Config::GetEnables("room");
	//for (auto room : rooms) {
	//	roomFactory->SetConfig(room, true);
	//}
}

void Map::InitTerrains(unordered_map<string, HMODULE>& modHandles,
	vector<string>& dlls) {
	terrainFactory->RegisterTerrain(EmptyTerrain::GetId(),
		[]() { return new EmptyTerrain(); },
		[](TerrainMod* terrain) { delete terrain; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModTerrainsFunc)GetProcAddress(modHandle, "RegisterModTerrains");
			if (registerFunc) {
				registerFunc(terrainFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}

#ifdef MOD_TEST
	auto terrainList = { "mod" };
	for (const auto& terrainId : terrainList) {
		if (terrainFactory->CheckRegistered(terrainId)) {
			auto terrain = terrainFactory->CreateTerrain(terrainId);
			debugf("Log: Created test terrain %s.\n", terrainId);
			terrainFactory->DestroyTerrains({ terrain });
		}
		else {
			debugf("Warning: Terrain %s not registered.\n", terrainId);
		}
	}
#endif
}

void Map::InitRoadnets(unordered_map<string, HMODULE>& modHandles,
	vector<string>& dlls) {
	roadnetFactory->RegisterRoadnet(EmptyRoadnet::GetId(),
		[]() { return new EmptyRoadnet(); },
		[](RoadnetMod* roadnet) { delete roadnet; });

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModRoadnetsFunc)GetProcAddress(modHandle, "RegisterModRoadnets");
			if (registerFunc) {
				registerFunc(roadnetFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}

#ifdef MOD_TEST
	auto roadnetList = { "mod" };
	for (const auto& roadnetId : roadnetList) {
		if (roadnetFactory->CheckRegistered(roadnetId)) {
			auto roadnet = roadnetFactory->CreateRoadnet(roadnetId);
			debugf("Log: Created test roadnet %s.\n", roadnetId);
			roadnetFactory->DestroyRoadnet(roadnet);
		}
		else {
			debugf("Warning: Roadnet %s not registered.\n", roadnetId);
		}
	}
#endif
}

void Map::InitZones(unordered_map<string, HMODULE>& modHandles,
	vector<string>& dlls) {
	zoneFactory->RegisterZone(EmptyZone::GetId(), EmptyZone::ZoneAssigner,
		[]() { return new EmptyZone(); },
		[](ZoneMod* zone) { delete zone; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModZonesFunc)GetProcAddress(modHandle, "RegisterModZones");
			if (registerFunc) {
				registerFunc(zoneFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}

#ifdef MOD_TEST
	auto zoneList = { "mod" };
	for (const auto& zoneId : zoneList) {
		if (zoneFactory->CheckRegistered(zoneId)) {
			auto zone = zoneFactory->CreateZone(zoneId);
			debugf("Log: Created test zone %s.\n", zoneId);
			zoneFactory->DestroyZone(zone);
		}
		else {
			debugf("Warning: Zone %s not registered.\n", zoneId);
		}
	}
#endif
}

void Map::InitBuildings(unordered_map<string, HMODULE>& modHandles,
	vector<string>& dlls) {
	buildingFactory->RegisterBuilding(EmptyBuilding::GetId(),
		EmptyBuilding::GetPowers(), EmptyBuilding::BuildingAssigner,
		[]() { return new EmptyBuilding(); },
		[](BuildingMod* building) { delete building; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModBuildingsFunc)GetProcAddress(modHandle, "RegisterModBuildings");
			if (registerFunc) {
				registerFunc(buildingFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}

#ifdef MOD_TEST
	auto buildingList = { "mod" };
	for (const auto& buildingId : buildingList) {
		if (buildingFactory->CheckRegistered(buildingId)) {
			auto building = buildingFactory->CreateBuilding(buildingId);
			debugf("Log: Created test building %s.\n", buildingId);
			buildingFactory->DestroyBuilding(building);
		}
		else {
			debugf("Warning: Building %s not registered.\n", buildingId);
		}
	}
#endif
}

int Map::Init(int chunkX, int chunkY) {
	Destroy();

	if (chunkX < 1 || chunkY < 1) {
		THROW_EXCEPTION(InvalidArgumentException, "Invalid map size.\n");
	}

	width = chunkX * CHUNK_SIZE;
	height = chunkY * CHUNK_SIZE;
	playerPos.first = width / 2.f;
	playerPos.second = height / 2.f;

	debugf("Log: Initializing map with size %d x %d (chunk size: %d x %d).\n", width, height, chunkX, chunkY);
	chunks = vector<vector<shared_ptr<Chunk>>>(chunkY,
		vector<shared_ptr<Chunk>>(chunkX, nullptr));
	for (int i = 0; i < chunkX; ++i) {
		for (int j = 0; j < chunkY; ++j) {
			chunks[j][i] = make_shared<Chunk>(i * CHUNK_SIZE, j * CHUNK_SIZE);
		}
	}

	debugf("Log: Generate terrains.\n");
	auto getTerrain = [this](int x, int y) -> string {
		return this->GetTerrain(x, y);
		};
	auto getHeight = [this](int x, int y) -> float {
		return this->GetHeight(x, y);
		};
	auto setElement = [this](int x, int y, string terrain, float height) -> bool {
		return this->SetTerrain(x, y, terrain, height);
		};
	auto terrainNames = terrainFactory->GetTerrains();
	vector<Terrain*> terrains;
	for (auto name : terrainNames) {
		terrains.push_back(new Terrain(terrainFactory, name));
	}
	sort(terrains.begin(), terrains.end(),
		[](const Terrain* a, const Terrain* b) {
			return a->GetPriority() > b->GetPriority();
		});
	for (auto& terrain : terrains) {
		terrain->DistributeTerrain(width, height, setElement, getTerrain, getHeight);
	}
	for (auto terrain : terrains) {
		delete terrain;
	}

	debugf("Log: Generate roadnet.\n");
	roadnet = new Roadnet(roadnetFactory, roadnetFactory->GetRoadnet()) ;
	if (!roadnet) {
		THROW_EXCEPTION(RuntimeException, "No enabled roadnet in config.\n");
	}
	roadnet->DistributeRoadnet(width, height, getTerrain);
	roadnet->AllocateAddress();

	debugf("Log: Generate zones.\n");
	auto zoneTypes = zoneFactory->GetTypes();
	for (auto block : roadnet->GetBlocks()) {
		if (!block) continue;
		vector<Zone*> zones;
		for (auto type : zoneTypes) {
			for (auto zone : zoneFactory->CreateZones(type, block)) {
				zones.push_back(new Zone(zoneFactory, type));
			}
		}
		for (auto zone : zones) {
			zone->LayoutZone(block, buildingFactory);
			string name = zone->GetName();
			block->AddZone(name, zone);
		}
		for (auto& zone : zones) {
			if (!zone) continue;
			zone->SetParent(block);
			for (auto& b : zone->GetBuildings()) {
				Building* building = b.second;
				if (!building) continue;
				building->SetParent(zone);
				building->SetParent(block);
			}
			if (this->zones.find(zone->GetName()) != this->zones.end()) {
				THROW_EXCEPTION(RuntimeException, "Duplicate zone name: " + zone->GetName() + ".\n");
			}
			this->zones[zone->GetName()] = zone;
		}
	}

	debugf("Log: Generate buildings.\n");
	auto powers = buildingFactory->GetPowers();
	vector<vector<pair<string, float>>> cdfs(AREA_END);
	for (int area = 0; area < AREA_END; ++area) {
		float sum = 0.f;
		for (auto& power : powers) {
			sum += power.second[area];
			cdfs[area].emplace_back(power.first, sum);
		}
		if (sum == 0.f) {
			cdfs[area].clear();
			continue;
		}
		for (auto& cdf : cdfs[area]) {
			cdf.second /= sum;
		}
	}
	for (auto block : roadnet->GetBlocks()) {
		if (!block) continue;
		float acreageBlock = block->GetAcreage();
		for (auto& zone : block->GetZones()) {
			if (zone.second) acreageBlock -= zone.second->GetQuad()->GetAcreage();
		}
		if (acreageBlock <= 0.f) continue;

		vector<string> presets;
		for (auto [type, _] : powers) {
			auto preset = buildingFactory->CreateBuildings(type, block);
			presets.insert(presets.end(), preset.begin(), preset.end());
		}
		float acreageTmp = 0.f;
		int attempt = 0;
		while (acreageTmp < acreageBlock) {
			if (attempt > 16) break;

			Building* building = nullptr;

			if (presets.size() > 0) {
				auto preset = presets.back();
				presets.pop_back();
				building = new Building(buildingFactory, preset);
			}
			else {
				float rand = GetRandom(int(1e5)) / 1e5f;
				for (auto& cdf : cdfs[block->GetArea()]) {
					if (rand < cdf.second) {
						building = new Building(buildingFactory, cdf.first);
						break;
					}
				}
			}

			if (!building) {
				++attempt;
				continue;
			}

			float acreageBuilding = building->RandomAcreage();
			float acreageMin = building->GetAcreageMin();
			float acreageMax = building->GetAcreageMax();
			if (acreageBlock - acreageTmp < acreageMin) {
				++attempt;
				continue;
			}
			else if (acreageBlock - acreageTmp < acreageBuilding) {
				acreageBuilding = acreageBlock - acreageTmp;
			}

			acreageTmp += acreageBuilding;
			building->GetQuad()->SetAcreage(acreageBuilding);
			building->SetParent(block);
			block->AddBuilding(building->GetName(), building);
			if (buildings.find(building->GetName()) != buildings.end()) {
				THROW_EXCEPTION(RuntimeException, "Duplicate building name: " + building->GetName() + ".\n");
			}
			buildings[building->GetName()] = building;
		}
	}

	return 0;
}

void Map::Destroy() {
	chunks.clear();
}

pair<int, int> Map::GetSize() const {
	return make_pair(width, height);
}

pair<float, float> Map::GetPlayerPos() const {
	return playerPos;
}

bool Map::CheckXY(int x, int y) const {
	if (x < 0 || y < 0 || x >= width || y >= height) {
		return false;
	}
	return true;
}

shared_ptr<Chunk> Map::GetChunk(int x, int y) const {
	if (!CheckXY(x, y)) {
		THROW_EXCEPTION(InvalidArgumentException, "Invalid chunk query position.\n");
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	if (chunkY < 0 || chunkY >= (int)chunks.size() || chunkX < 0 || chunkX >= (int)chunks[0].size()) {
		THROW_EXCEPTION(OutOfRangeException, "Chunk index out of range.\n");
	}
	return chunks[chunkY][chunkX];
}

shared_ptr<Element> Map::GetElement(int x, int y) const {
	if (!CheckXY(x, y)) {
		THROW_EXCEPTION(InvalidArgumentException, "Invalid element query position.\n");
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	if (chunkY < 0 || chunkY >= (int)chunks.size() || chunkX < 0 || chunkX >= (int)chunks[0].size()) {
		THROW_EXCEPTION(OutOfRangeException, "Chunk index out of range.\n");
	}
	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		THROW_EXCEPTION(NullPointerException, "Chunk is null.\n");
	}
	return chunk->GetElement(x, y);
}

string Map::GetTerrain(int x, int y) const {
	if (!CheckXY(x, y)) {
		return "";
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	if (chunkY >= (int)chunks.size() || chunkX >= (int)chunks[0].size()) {
		return "";
	}
	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) return "";
	return chunk->GetTerrain(x, y);
}

bool Map::SetTerrain(int x, int y, const string& terrain, float height) {
	if (!CheckXY(x, y)) {
		return false;
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	if (chunkY >= (int)chunks.size() || chunkX >= (int)chunks[0].size()) {
		return false;
	}
	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) return false;
	return chunk->SetTerrain(x, y, terrain, height);
}

float Map::GetHeight(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid map coordinates (%d, %d).\n", x, y);
		return 0.f;
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	if (chunkY >= (int)chunks.size() || chunkX >= (int)chunks[0].size()) {
		debugf("Warning: Chunk index out of range (%d, %d).\n", chunkX, chunkY);
		return 0.f;
	}
	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk is null at (%d, %d).\n", chunkX, chunkY);
		return 0.f;
	}
	return chunk->GetHeight(x, y);
}

Roadnet* Map::GetRoadnet() const {
	return roadnet;
}

