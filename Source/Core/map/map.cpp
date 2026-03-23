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
ComponentFactory* Map::componentFactory = nullptr;
RoomFactory* Map::roomFactory = nullptr;

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
	height(0),
	chunks(),
	playerPos(0, 0),
	roadnet(nullptr),
	zones(),
	buildings(),
	layout(nullptr) {
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
	if (!componentFactory) {
		componentFactory = new ComponentFactory();
	}
	if (!roomFactory) {
		roomFactory = new RoomFactory();
	}
}

Map::~Map() {
	delete roadnet;
}

void Map::LoadConfigs() const {
	terrainFactory->RemoveAll();
	roadnetFactory->RemoveAll();
	zoneFactory->RemoveAll();
	buildingFactory->RemoveAll();
	componentFactory->RemoveAll();
	roomFactory->RemoveAll();

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
	auto components = Config::GetEnables("component");
	for (auto component : components) {
		componentFactory->SetConfig(component, true);
	}
	auto rooms = Config::GetEnables("room");
	for (auto room : rooms) {
		roomFactory->SetConfig(room, true);
	}
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
			terrainFactory->DestroyTerrain(terrain);
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

void Map::InitComponents(unordered_map<string, HMODULE>& modHandles,
	vector<string>& dlls) {
	componentFactory->RegisterComponent(EmptyComponent::GetId(),
		[]() { return new EmptyComponent(); },
		[](ComponentMod* building) { delete building; }
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

			auto registerFunc = (RegisterModComponentsFunc)GetProcAddress(modHandle, "RegisterModComponents");
			if (registerFunc) {
				registerFunc(componentFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}

#ifdef MOD_TEST
	auto componentList = { "mod" };
	for (const auto& componentId : componentList) {
		if (componentFactory->CheckRegistered(componentId)) {
			auto component = componentFactory->CreateComponent(componentId);
			debugf("Log: Created test component %s.\n", componentId);
			componentFactory->DestroyComponent(component);
		}
		else {
			debugf("Warning: Component %s not registered.\n", componentId);
		}
	}
#endif
}

void Map::InitRooms(unordered_map<string, HMODULE>& modHandles,
	vector<string>& dlls) {
	roomFactory->RegisterRoom(EmptyRoom::GetId(),
		[]() { return new EmptyRoom(); },
		[](RoomMod* room) { delete room; }
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

			auto registerFunc = (RegisterModRoomsFunc)GetProcAddress(modHandle, "RegisterModRooms");
			if (registerFunc) {
				registerFunc(roomFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}

#ifdef MOD_TEST
	auto roomList = { "mod" };
	for (const auto& roomId : roomList) {
		if (roomFactory->CheckRegistered(roomId)) {
			auto room = roomFactory->CreateRoom(roomId);
			debugf("Log: Created test room %s.\n", roomId);
			roomFactory->DestroyRoom(room);
		}
		else {
			debugf("Warning: Room %s not registered.\n", roomId);
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

	debugf("Log: Arranging zones and buildings.\n");
	ArrangeBlocks();
	for (auto block : roadnet->GetBlocks()) {
		if (!block) continue;
		auto zones = block->GetZones();
		for (auto& [name, zone] : zones) {
			if (!zone) continue;
			zone->ArrangeBuildings();
			SetZone(zone, name);
			for (auto& building : zone->GetBuildings()) {
				if (!building.second) continue;
				SetBuilding(building.second, building.first,
					make_pair(zone->GetQuad()->GetLeft(), zone->GetQuad()->GetBottom()));
			}
		}
		auto buildings = block->GetBuildings();
		for (auto& [name, building] : buildings) {
			if (!building) continue;
			SetBuilding(building, name);
		}
	}

	debugf("Log: Generate components and rooms.\n");
	int capacity = 0;
	if (layout) delete layout;
	layout = Building::ReadTemplates(Config::GetLayouts());
	for (auto& [name, building] : buildings) {
		if (!building) continue;
		building->PlaceConstruction();
		building->LayoutBuilding(layout, componentFactory, roomFactory);
		for (auto component : building->GetComponents()) {
			if (!component) continue;
			component->SetParent(building);
			for (auto room : component->GetRooms()) {
				if (!room) continue;
				room->SetParent(component);
				room->SetParent(building);
				capacity += room->ResidentialCapacity();
			}
		}
	}
	for (auto& [name, zone] : zones) {
		if (!zone) continue;
		for (auto& [name, building] : zone->GetBuildings()) {
			if (!building) continue;
			building->PlaceConstruction();
			building->LayoutBuilding(layout, componentFactory, roomFactory);
			for (auto component : building->GetComponents()) {
				if (!component) continue;
				component->SetParent(building);
				for (auto room : component->GetRooms()) {
					if (!room) continue;
					room->SetParent(component);
					room->SetParent(building);
					capacity += room->ResidentialCapacity();
				}
			}
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

unordered_map<string, Zone*>& Map::GetZones() {
	return zones;
}

unordered_map<string, Building*>& Map::GetBuildings() {
	return buildings;
}

Zone* Map::GetZone(const string& name) const {
	auto it = zones.find(name);
	if (it == zones.end()) return nullptr;
	return it->second;
}

Building* Map::GetBuilding(const string& name) const {
	auto it = buildings.find(name);
	if (it == buildings.end()) {
		for (auto& [_, zone] : zones) {
			auto building = zone->GetBuilding(name.data());
			if (building)return building;
		}
		return nullptr;
	}
	return it->second;
}

void Map::SetZone(Zone* zone, const string& name) {
	if (!zone) {
		THROW_EXCEPTION(NullPointerException, "Zone is null.\n");
	}
	auto block = zone->GetParent();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Zone's parent block is null.\n");
	}

	// 计算四个顶点（世界坐标）
	auto v1 = block->GetPosition(zone->GetQuad()->GetPosX() + zone->GetQuad()->GetSizeX() / 2.f, zone->GetQuad()->GetPosY() + zone->GetQuad()->GetSizeY() / 2.f);
	auto v2 = block->GetPosition(zone->GetQuad()->GetPosX() - zone->GetQuad()->GetSizeX() / 2.f, zone->GetQuad()->GetPosY() + zone->GetQuad()->GetSizeY() / 2.f);
	auto v3 = block->GetPosition(zone->GetQuad()->GetPosX() - zone->GetQuad()->GetSizeX() / 2.f, zone->GetQuad()->GetPosY() - zone->GetQuad()->GetSizeY() / 2.f);
	auto v4 = block->GetPosition(zone->GetQuad()->GetPosX() + zone->GetQuad()->GetSizeX() / 2.f, zone->GetQuad()->GetPosY() - zone->GetQuad()->GetSizeY() / 2.f);

	vector<pair<float, float>> points = { v1, v2, v3, v4 };
	int minX = (int)points[0].first;
	int maxX = (int)points[0].first;
	int minY = (int)points[0].second;
	int maxY = (int)points[0].second;
	for (const auto& [x, y] : points) { // 结构化绑定
		minX = min(minX, (int)x);
		maxX = max(maxX, (int)x);
		minY = min(minY, (int)y);
		maxY = max(maxY, (int)y);
	}

	for (int x = minX; x <= maxX; ++x) {
		for (int y = minY; y <= maxY; ++y) {
			bool inside = false;
			for (size_t i = 0, j = points.size() - 1; i < points.size(); j = i++) {
				const auto& [x1, y1] = points[i];
				const auto& [x2, y2] = points[j];
				if (((y1 > y) != (y2 > y)) &&
					(x < (x2 - x1) * (y - y1) / (y2 - y1) + x1)) {
					inside = !inside;
				}
			}
			if (inside) {
				auto element = GetElement(x, y);
				if (element) {
					element->SetZone(name);
				}
			}
		}
	}
}

void Map::SetBuilding(Building* building, const string& name) {
	if (!building) {
		THROW_EXCEPTION(NullPointerException, "Building is null.\n");
	}
	auto block = building->GetParentBlock();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Building's parent block is null.\n");
	}

	auto v1 = block->GetPosition(building->GetQuad()->GetPosX() + building->GetQuad()->GetSizeX() / 2.f, building->GetQuad()->GetPosY() + building->GetQuad()->GetSizeY() / 2.f);
	auto v2 = block->GetPosition(building->GetQuad()->GetPosX() - building->GetQuad()->GetSizeX() / 2.f, building->GetQuad()->GetPosY() + building->GetQuad()->GetSizeY() / 2.f);
	auto v3 = block->GetPosition(building->GetQuad()->GetPosX() - building->GetQuad()->GetSizeX() / 2.f, building->GetQuad()->GetPosY() - building->GetQuad()->GetSizeY() / 2.f);
	auto v4 = block->GetPosition(building->GetQuad()->GetPosX() + building->GetQuad()->GetSizeX() / 2.f, building->GetQuad()->GetPosY() - building->GetQuad()->GetSizeY() / 2.f);

	vector<pair<float, float>> points = { v1, v2, v3, v4 };
	int minX = (int)points[0].first;
	int maxX = (int)points[0].first;
	int minY = (int)points[0].second;
	int maxY = (int)points[0].second;
	for (const auto& [x, y] : points) {
		minX = min(minX, (int)x);
		maxX = max(maxX, (int)x);
		minY = min(minY, (int)y);
		maxY = max(maxY, (int)y);
	}

	for (int x = minX; x <= maxX; ++x) {
		for (int y = minY; y <= maxY; ++y) {
			bool inside = false;
			for (size_t i = 0, j = points.size() - 1; i < points.size(); j = i++) {
				const auto& [x1, y1] = points[i];
				const auto& [x2, y2] = points[j];
				if (((y1 > y) != (y2 > y)) &&
					(x < (x2 - x1) * (y - y1) / (y2 - y1) + x1)) {
					inside = !inside;
				}
			}
			if (inside) {
				auto element = GetElement(x, y);
				if (element) {
					element->SetBuilding(name);
				}
			}
		}
	}
}

void Map::SetBuilding(Building* building, const string& name, pair<float, float> offset) {
	if (!building) {
		THROW_EXCEPTION(NullPointerException, "Building is null.\n");
	}
	auto block = building->GetParentBlock();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Building's parent block is null.\n");
	}

	auto v1 = block->GetPosition(offset.first + building->GetQuad()->GetPosX() + building->GetQuad()->GetSizeX() / 2.f, offset.second + building->GetQuad()->GetPosY() + building->GetQuad()->GetSizeY() / 2.f);
	auto v2 = block->GetPosition(offset.first + building->GetQuad()->GetPosX() - building->GetQuad()->GetSizeX() / 2.f, offset.second + building->GetQuad()->GetPosY() + building->GetQuad()->GetSizeY() / 2.f);
	auto v3 = block->GetPosition(offset.first + building->GetQuad()->GetPosX() - building->GetQuad()->GetSizeX() / 2.f, offset.second + building->GetQuad()->GetPosY() - building->GetQuad()->GetSizeY() / 2.f);
	auto v4 = block->GetPosition(offset.first + building->GetQuad()->GetPosX() + building->GetQuad()->GetSizeX() / 2.f, offset.second + building->GetQuad()->GetPosY() - building->GetQuad()->GetSizeY() / 2.f);

	vector<pair<float, float>> points = { v1, v2, v3, v4 };
	int minX = (int)points[0].first;
	int maxX = (int)points[0].first;
	int minY = (int)points[0].second;
	int maxY = (int)points[0].second;
	for (const auto& [x, y] : points) {
		minX = min(minX, (int)x);
		maxX = max(maxX, (int)x);
		minY = min(minY, (int)y);
		maxY = max(maxY, (int)y);
	}

	for (int x = minX; x <= maxX; ++x) {
		for (int y = minY; y <= maxY; ++y) {
			bool inside = false;
			for (size_t i = 0, j = points.size() - 1; i < points.size(); j = i++) {
				const auto& [x1, y1] = points[i];
				const auto& [x2, y2] = points[j];
				if (((y1 > y) != (y2 > y)) &&
					(x < (x2 - x1) * (y - y1) / (y2 - y1) + x1)) {
					inside = !inside;
				}
			}
			if (inside) {
				auto element = GetElement(x, y);
				if (element) {
					element->SetBuilding(name);
				}
			}
		}
	}
}

void Map::ArrangeBlocks() {
	if (!roadnet) return;
	auto blocks = roadnet->GetBlocks();
	for (auto block : blocks) {
		if (!block) continue;
		auto zones = block->GetZones();
		auto buildings = block->GetBuildings();

		if (zones.empty() && buildings.empty()) continue;

		float acreageTotal = block->GetAcreage();
		float acreageUsed = 0.f;

		for (const auto& [name, zone] : zones) {
			if (zone) acreageUsed += zone->GetQuad()->GetAcreage();
		}
		for (const auto& [name, building] : buildings) {
			if (building) acreageUsed += building->GetQuad()->GetAcreage();
		}
		float acreageRemain = acreageTotal - acreageUsed;

		bool acreageAllocate = false;
		if (acreageRemain > 0) {
			for (auto& [name, building] : buildings) {
				if (!building) continue;
				float acreageTmp = building->RandomAcreage();
				float acreageMax = building->GetAcreageMax();
				float acreageMin = building->GetAcreageMin();

				float acreageExpand = acreageMax - acreageTmp;

				if (acreageExpand > acreageRemain && acreageRemain > 0) {
					float acreageNew = acreageTmp + acreageRemain;
					if (acreageNew >= acreageMin && acreageNew <= acreageMax) {
						building->GetQuad()->SetAcreage(acreageNew);
						acreageUsed += acreageRemain;
						acreageRemain = 0.f;
						acreageAllocate = true;
						break;
					}
				}
			}
		}

		Quad* emptyRect = nullptr;
		vector<Quad*> elements;
		if (acreageRemain > 0 && !acreageAllocate) {
			emptyRect = new Block();
			emptyRect->SetAcreage(acreageRemain);
			elements.push_back(emptyRect);
		}

		for (const auto& [name, zone] : zones) {
			if (zone) elements.push_back(zone->GetQuad());
		}
		for (const auto& [name, building] : buildings) {
			if (building) elements.push_back(building->GetQuad());
		}

		if (elements.empty()) continue;

		sort(elements.begin(), elements.end(), [](Quad* a, Quad* b) {
			return a->GetAcreage() > b->GetAcreage();
			});

		Quad container = Quad(block->GetSizeX() / 2, block->GetSizeY() / 2, block->GetSizeX(), block->GetSizeY());
		if (elements.size() == 1) {
			elements[0]->SetPosition(container.GetPosX(), container.GetPosY(), container.GetSizeX(), container.GetSizeY());
		}
		else {
			class Chunk : public Quad {
			public:
				Chunk(Quad* r1, Quad* r2) : r1(r1), r2(r2) { acreage = r1->GetAcreage() + r2->GetAcreage(); }
				Quad* r1, * r2;
			};
			while (elements.size() > 2) {
				Chunk* tmp = new Chunk(elements[elements.size() - 1], elements[elements.size() - 2]);
				elements.pop_back();
				int i = (int)elements.size() - 2;
				for (; i >= 0; --i) {
					if (tmp->GetAcreage() > elements[i]->GetAcreage()) {
						elements[i + 1] = elements[i];
					}
					else {
						elements[i + 1] = tmp;
						break;
					}
				}
				if (i < 0) elements[0] = tmp;
			}

			if (container.GetSizeX() > container.GetSizeY()) {
				if (GetRandom(2)) {
					int divX = int(container.GetLeft() +
						(container.GetRight() - container.GetLeft()) * elements[0]->GetAcreage() / container.GetAcreage());
					if (abs(divX - container.GetLeft()) < 2) divX = (int)container.GetLeft();
					if (abs(divX - container.GetRight()) < 2) divX = (int)container.GetRight();
					elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
					elements[1]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
				}
				else {
					int divX = int(container.GetLeft() +
						(container.GetRight() - container.GetLeft()) * elements[1]->GetAcreage() / container.GetAcreage());
					if (abs(divX - container.GetLeft()) < 2) divX = (int)container.GetLeft();
					if (abs(divX - container.GetRight()) < 2) divX = (int)container.GetRight();
					elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
					elements[0]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
				}
			}
			else {
				if (GetRandom(2)) {
					int divY = int(container.GetBottom() +
						(container.GetTop() - container.GetBottom()) * elements[0]->GetAcreage() / container.GetAcreage());
					if (abs(divY - container.GetBottom()) < 2) divY = (int)container.GetBottom();
					if (abs(divY - container.GetTop()) < 2) divY = (int)container.GetTop();
					elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
					elements[1]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
				}
				else {
					int divY = int(container.GetBottom() +
						(container.GetTop() - container.GetBottom()) * elements[1]->GetAcreage() / container.GetAcreage());
					if (abs(divY - container.GetBottom()) < 2) divY = (int)container.GetBottom();
					if (abs(divY - container.GetTop()) < 2) divY = (int)container.GetTop();
					elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
					elements[0]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
				}
			}

			while (!elements.empty()) {
				auto tmp = elements.back();
				elements.pop_back();
				if (auto chunk = dynamic_cast<Chunk*>(tmp)) {
					Quad* rect1 = chunk->r1;
					Quad* rect2 = chunk->r2;

					if (tmp->GetAcreage() > 0) {
						if (tmp->GetSizeX() > tmp->GetSizeY()) {
							if (GetRandom(2)) {
								int divX = int(tmp->GetLeft() +
									tmp->GetSizeX() * rect1->GetAcreage() / tmp->GetAcreage());
								if (abs(divX - tmp->GetLeft()) < 2) divX = (int)tmp->GetLeft();
								if (abs(divX - tmp->GetRight()) < 2) divX = (int)tmp->GetRight();
								rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
								rect2->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
							}
							else {
								int divX = int(tmp->GetLeft() +
									tmp->GetSizeX() * rect2->GetAcreage() / tmp->GetAcreage());
								if (abs(divX - tmp->GetLeft()) < 2) divX = (int)tmp->GetLeft();
								if (abs(divX - tmp->GetRight()) < 2) divX = (int)tmp->GetRight();
								rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
								rect1->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
							}
						}
						else {
							if (GetRandom(2)) {
								int divY = int(tmp->GetBottom() +
									tmp->GetSizeY() * rect1->GetAcreage() / tmp->GetAcreage());
								if (abs(divY - tmp->GetBottom()) < 2) divY = (int)tmp->GetBottom();
								if (abs(divY - tmp->GetTop()) < 2) divY = (int)tmp->GetTop();
								rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
								rect2->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
							}
							else {
								int divY = int(tmp->GetBottom() +
									tmp->GetSizeY() * rect2->GetAcreage() / tmp->GetAcreage());
								if (abs(divY - tmp->GetBottom()) < 2) divY = (int)tmp->GetBottom();
								if (abs(divY - tmp->GetTop()) < 2) divY = (int)tmp->GetTop();
								rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
								rect1->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
							}
						}
						if (dynamic_cast<Chunk*>(rect1)) elements.push_back(rect1);
						if (dynamic_cast<Chunk*>(rect2)) elements.push_back(rect2);
					}
					delete chunk;
				}
			}
		}

		if (emptyRect) {
			delete emptyRect;
		}
	}
}

