#include "map.h"

#include "map/block.h"
#include "map/terrain.h"
#include "map/roadnet.h"
#include "map/zone.h"
#include "map/building.h"
#include "map/component.h"
#include "map/room.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "populace/asset.h"
#include "story/change.h"
#include "player/player.h"

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

bool Element::SetTerrain(const string& terrain) {
	this->terrain = terrain;
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
	elements = vector<vector<Element*>>(CHUNK_SIZE, vector<Element*>(CHUNK_SIZE, nullptr));
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			elements[j][i] = new Element();
		}
	}
}

Chunk::~Chunk() {
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			delete elements[j][i];
		}
	}
	elements.clear();
}

string Chunk::GetTerrain(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return "";
	}
	return elements[y - offsetY][x - offsetX]->GetTerrain();
}

bool Chunk::SetTerrain(int x, int y, const string& terrain) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return false;
	}
	return elements[y - offsetY][x - offsetX]->SetTerrain(terrain);
}

float Chunk::GetHeight(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return 0.f;
	}
	return elements[y - offsetY][x - offsetX]->GetHeight();
}

bool Chunk::SetHeight(int x, int y, float height) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return false;
	}
	return elements[y - offsetY][x - offsetX]->SetHeight(height);
}

string Chunk::GetZone(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return "";
	}
	return elements[y - offsetY][x - offsetX]->GetZone();
}

bool Chunk::SetZone(int x, int y, const string& zone) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return false;
	}
	return elements[y - offsetY][x - offsetX]->SetTerrain(zone);
}

string Chunk::GetBuilding(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return "";
	}
	return elements[y - offsetY][x - offsetX]->GetBuilding();
}

bool Chunk::SetBuilding(int x, int y, const string& building) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return false;
	}
	return elements[y - offsetY][x - offsetX]->SetBuilding(building);
}

bool Chunk::CheckXY(int x, int y) const {
	if (x < offsetX) return false;
	if (y < offsetY) return false;
	if (x >= offsetX + CHUNK_SIZE) return false;
	if (y >= offsetY + CHUNK_SIZE) return false;
	return true;
}

Map::Map() :
	width(0),
	height(0),
	chunks(),
	playerPos(0.f, 0.f),
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
	Destroy();
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
	const vector<string>& dlls) {
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
}

void Map::InitRoadnets(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
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
}

void Map::InitZones(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
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
}

void Map::InitBuildings(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
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
}

void Map::InitComponents(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
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
}

void Map::InitRooms(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
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
	chunks = vector<vector<Chunk*>>(chunkY, vector<Chunk*>(chunkX, nullptr));
	for (int i = 0; i < chunkX; ++i) {
		for (int j = 0; j < chunkY; ++j) {
			chunks[j][i] = new Chunk(i * CHUNK_SIZE, j * CHUNK_SIZE);
		}
	}

	debugf("Log: Generate terrains.\n");
	auto getTerrain = [this](int x, int y) -> string {
		return this->GetTerrain(x, y);
		};
	auto setTerrain = [this](int x, int y, string terrain) -> bool {
		return this->SetTerrain(x, y, terrain);
		};
	auto getHeight = [this](int x, int y) -> float {
		return this->GetHeight(x, y);
		};
	auto setHeight = [this](int x, int y, float height) -> bool {
		return this->SetHeight(x, y, height);
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
		terrain->DistributeTerrain(width, height, getTerrain, setTerrain, getHeight, setHeight);
	}
	for (auto &terrain : terrains) {
		delete terrain;
	}
	terrains.clear();

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
			zone->LayoutZone(block);
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
			if (zone.second) acreageBlock -= zone.second->GetAcreage();
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
				delete building;
				continue;
			}
			else if (acreageBlock - acreageTmp < acreageBuilding) {
				acreageBuilding = acreageBlock - acreageTmp;
			}

			acreageTmp += acreageBuilding;
			building->SetAcreage(acreageBuilding);
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
	ClearZero();
	for (auto block : roadnet->GetBlocks()) {
		if (!block) continue;
		auto zones = block->GetZones();
		for (auto& [name, zone] : zones) {
			if (!zone) continue;
			zone->ArrangeBuildings();
			zone->ClearZero();
			SetZone(zone, name);
			for (auto& building : zone->GetBuildings()) {
				if (!building.second) continue;
				SetBuilding(building.second, building.first,
					make_pair(zone->GetLeft(), zone->GetBottom()));
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
		building->LayoutBuilding(layout);
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
			building->LayoutBuilding(layout);
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
	return capacity / 2;
}

void Map::Checkin(Populace* populace, Player* player) const {
	auto citizens = populace->GetCitizens();
	auto time = player->GetTime();

	// 筛选成年市民
	vector<Person*> adults;
	adults.reserve(citizens.size());
	for (auto citizen : citizens) {
		if (citizen && citizen->GetAge(time) >= 18) {
			adults.push_back(citizen);
		}
	}
	if (adults.empty()) return;

	// 为房产分配房东
	vector<pair<Room*, int>> residences;
	for (auto& zonePair : zones) {
		Zone* zone = zonePair.second;
		if (!zone) continue;
		if (zone->GetStated()) {
			for (auto& buildingPair : zone->GetBuildings()) {
				Building* building = buildingPair.second;
				if (!building) continue;
				building->SetStated(true);
				for (auto room : building->GetRooms()) {
					if (!room) continue;
					room->SetStated(true);
					if (room->IsResidential()) {
						residences.emplace_back(room, 0);
					}
				}
			}
			continue;
		}

		if (GetRandom(100) < 2) {
			int index = GetRandom(int(adults.size()));
			zone->SetOwner(adults[index]);
			auto asset = new Asset(Populace::assetFactory, "zone");
			if (asset) {
				asset->SetAsset(zone->GetAddress());
				adults[index]->AddAsset(asset);
			}
			for (auto& buildingPair : zone->GetBuildings()) {
				Building* building = buildingPair.second;
				if (!building) continue;
				building->SetOwner(adults[index]);
				for (auto room : building->GetRooms()) {
					if (!room) continue;
					room->SetOwner(adults[index]);
					if (room->IsResidential()) {
						residences.emplace_back(room, 0);
					}
				}
			}
		}
		else {
			for (auto& buildingPair : zone->GetBuildings()) {
				Building* building = buildingPair.second;
				if (!building) continue;
				if (GetRandom(100) < 5) {
					int index = GetRandom(int(adults.size()));
					building->SetOwner(adults[index]);
					auto asset = new Asset(Populace::assetFactory, "building");
					if (asset) {
						asset->SetAsset(building->GetAddress());
						adults[index]->AddAsset(asset);
					}
					for (auto room : building->GetRooms()) {
						if (!room) continue;
						room->SetOwner(adults[index]);
						if (room->IsResidential()) {
							residences.emplace_back(room, 0);
						}
					}
				}
				else {
					for (auto room : building->GetRooms()) {
						if (!room) continue;
						int index = GetRandom(int(adults.size()));
						room->SetOwner(adults[index]);
						auto asset = new Asset(Populace::assetFactory, "room");
						if (asset) {
							asset->SetAsset(room->GetAddress());
							adults[index]->AddAsset(asset);
						}
						if (room->IsResidential()) {
							residences.emplace_back(room, 0);
						}
					}
				}
			}
		}
	}

	for (auto& [name, building] : buildings) {
		if (!building) continue;
		if (building->GetStated()) {
			for (auto room : building->GetRooms()) {
				if (!room) continue;
				room->SetStated(true);
				if (room->IsResidential()) {
					residences.emplace_back(room, 0);
				}
			}
			continue;
		}

		if (GetRandom(100) < 5) {
			int index = GetRandom(int(adults.size()));
			building->SetOwner(adults[index]);
			auto asset = new Asset(Populace::assetFactory, "building");
			if (asset) {
				asset->SetAsset(building->GetAddress());
				adults[index]->AddAsset(asset);
			}
			for (auto room : building->GetRooms()) {
				if (!room) continue;
				room->SetOwner(adults[index]);
				if (room->IsResidential()) {
					residences.emplace_back(room, 0);
				}
			}
		}
		else {
			for (auto room : building->GetRooms()) {
				if (!room) continue;
				int index = GetRandom(int(adults.size()));
				room->SetOwner(adults[index]);
				auto asset = new Asset(Populace::assetFactory, "room");
				if (asset) {
					asset->SetAsset(room->GetAddress());
					adults[index]->AddAsset(asset);
				}
				if (room->IsResidential()) {
					residences.emplace_back(room, 0);
				}
			}
		}
	}

	// 分配市民住所
	for (auto adult : adults) {
		if (!adult) continue;
		if (adult->GetHome()) continue;
		if (residences.empty()) break;

		int index = GetRandom((int)residences.size());
		auto& [room, count] = residences[index];

		int num = 1;
		adult->SetHome(room);
		adult->SetStatus(room);
		room->AddTenant(adult);
		if (adult->GetSpouse() && adult->GetSpouse()->GetHome() == nullptr) {
			if (GetRandom(10) > 0) {
				adult->GetSpouse()->SetHome(room);
				adult->GetSpouse()->SetStatus(room);
				room->AddTenant(adult->GetSpouse());
				++num;
				for (auto child : adult->GetChilds()) {
					if (child && child->GetAge(time) < 18) {
						child->SetHome(room);
						child->SetStatus(room);
						room->AddTenant(child);
						++num;
					}
				}
			}
		}

		residences[index] = residences.back();
		residences.pop_back();
	}
}

void Map::Destroy() {
	for (auto &row : chunks) {
		for (auto &chunk : row) {
			delete chunk;
		}
	}
	chunks.clear();

	zones.clear();
	buildings.clear();

	if(roadnet)delete roadnet;
	roadnet = nullptr;

	if(layout)delete layout;
	layout = nullptr;
}

void Map::Tick(Player* player) {

}

void Map::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}

pair<int, int> Map::GetSize() const {
	return make_pair(width, height);
}

string Map::GetTerrain(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return "";
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return "";
	}
	return chunk->GetTerrain(x, y);
}

bool Map::SetTerrain(int x, int y, const string& terrain) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return false;
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return false;
	}
	return chunk->SetTerrain(x, y, terrain);
}

float Map::GetHeight(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return 0.f;
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return 0.f;
	}
	return chunk->GetHeight(x, y);
}

bool Map::SetHeight(int x, int y, float height) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return false;
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return false;
	}
	return chunk->SetHeight(x, y, height);
}

string Map::GetZone(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return "";
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return "";
	}
	return chunk->GetZone(x, y);
}

bool Map::SetZone(int x, int y, const string& zone) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return false;
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return false;
	}
	return chunk->SetZone(x, y, zone);
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
	auto v1 = block->GetPosition(zone->GetPosX() + zone->GetSizeX() / 2.f,
		zone->GetPosY() + zone->GetSizeY() / 2.f);
	auto v2 = block->GetPosition(zone->GetPosX() - zone->GetSizeX() / 2.f,
		zone->GetPosY() + zone->GetSizeY() / 2.f);
	auto v3 = block->GetPosition(zone->GetPosX() - zone->GetSizeX() / 2.f,
		zone->GetPosY() - zone->GetSizeY() / 2.f);
	auto v4 = block->GetPosition(zone->GetPosX() + zone->GetSizeX() / 2.f,
		zone->GetPosY() - zone->GetSizeY() / 2.f);

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
				SetZone(x, y, name);
			}
		}
	}
}

string Map::GetBuilding(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return "";
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return "";
	}
	return chunk->GetBuilding(x, y);
}

bool Map::SetBuilding(int x, int y, const string& building) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return false;
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return "";
	}
	return chunk->SetBuilding(x, y, building);
}

void Map::SetBuilding(Building* building, const string& name, const pair<float, float>& offset) {
	if (!building) {
		THROW_EXCEPTION(NullPointerException, "Building is null.\n");
	}
	auto block = building->GetParentBlock();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Building's parent block is null.\n");
	}

	auto v1 = block->GetPosition(offset.first + building->GetPosX() + building->GetSizeX() / 2.f,
		offset.second + building->GetPosY() + building->GetSizeY() / 2.f);
	auto v2 = block->GetPosition(offset.first + building->GetPosX() - building->GetSizeX() / 2.f,
		offset.second + building->GetPosY() + building->GetSizeY() / 2.f);
	auto v3 = block->GetPosition(offset.first + building->GetPosX() - building->GetSizeX() / 2.f,
		offset.second + building->GetPosY() - building->GetSizeY() / 2.f);
	auto v4 = block->GetPosition(offset.first + building->GetPosX() + building->GetSizeX() / 2.f,
		offset.second + building->GetPosY() - building->GetSizeY() / 2.f);

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
				SetBuilding(x, y, name);
			}
		}
	}
}

pair<float, float> Map::GetPlayerPos() const {
	return playerPos;
}

Roadnet* Map::GetRoadnet() const {
	return roadnet;
}

const unordered_map<string, Zone*>& Map::GetZones() const {
	return zones;
}

Zone* Map::GetZone(const string& name) const {
	auto it = zones.find(name);
	if (it == zones.end()) return nullptr;
	return it->second;
}

const unordered_map<string, Building*>& Map::GetBuildings() const {
	return buildings;
}

Building* Map::GetBuilding(const string& name) const {
	auto it = buildings.find(name);
	if (it == buildings.end()) {
		for (auto& [_, zone] : zones) {
			auto building = zone->GetBuilding(name);
			if (building)return building;
		}
		return nullptr;
	}
	return it->second;
}

vector<Component*> Map::GetComponents() const {
	vector<Component*> components;
	for (const auto& [name, zone] : zones) {
		if (!zone) continue;
		for (auto [name, building] : zone->GetBuildings()) {
			if (!building) continue;
			const auto& current = building->GetComponents();
			components.insert(components.end(), current.begin(), current.end());
		}
	}
	for (const auto& [name, building] : buildings) {
		if (!building) continue;
		const auto& current = building->GetComponents();
		components.insert(components.end(), current.begin(), current.end());
	}
	return components;
}

vector<Room*> Map::GetRooms() const {
	vector<Room*> rooms;
	for (const auto& [name, zone] : zones) {
		if (!zone) continue;
		for (auto [name, building] : zone->GetBuildings()) {
			if (!building) continue;
			const auto& current = building->GetRooms();
			rooms.insert(rooms.end(), current.begin(), current.end());
		}
	}
	for (const auto& [name, building] : buildings) {
		if (!building) continue;
		const auto& current = building->GetRooms();
		rooms.insert(rooms.end(), current.begin(), current.end());
	}
	return rooms;
}

Block* Map::LocateBlock(const string& address) const {
	istringstream iss(address);
	string road;
	int id = -1;
	iss >> road >> id;
	if (id < 0 || !roadnet) return nullptr;
	return roadnet->LocateBlock(road, id);
}

Zone* Map::LocateZone(const string& address) const {
	istringstream iss(address);
	string road;
	int id = -1;
	iss >> road >> id;
	if (id < 0 || !roadnet) return nullptr;
	auto block = roadnet->LocateBlock(road, id);
	if (!block) return nullptr;

	string zone;
	iss >> zone;
	auto& blockZones = block->GetZones();
	auto it = blockZones.find(zone);
	if (it == blockZones.end()) return nullptr;
	return it->second;
}

Building* Map::LocateBuilding(const string& address) const {
	istringstream iss(address);
	string road;
	int id = -1;
	iss >> road >> id;
	if (id < 0 || !roadnet) return nullptr;
	auto block = roadnet->LocateBlock(road, id);
	if (!block) return nullptr;

	string token;
	iss >> token;
	auto& blockZones = block->GetZones();
	auto it = blockZones.find(token);
	if (it == blockZones.end()) {
		auto& blockBuildings = block->GetBuildings();
		auto bit = blockBuildings.find(token);
		if (bit == blockBuildings.end()) return nullptr;
		return bit->second;
	}
	else {
		Zone* zone = it->second;
		if (!zone) return nullptr;
		string building;
		iss >> building;
		auto& zoneBuildings = zone->GetBuildings();
		auto bit = zoneBuildings.find(building);
		if (bit == zoneBuildings.end()) return nullptr;
		return bit->second;
	}
}

Room* Map::LocateRoom(const string& address) const {
	istringstream iss(address);
	string road;
	int id = -1;
	iss >> road >> id;
	if (id < 0 || !roadnet) return nullptr;
	auto block = roadnet->LocateBlock(road, id);
	if (!block) return nullptr;

	string token;
	iss >> token;
	auto& blockZones = block->GetZones();
	auto it = blockZones.find(token);
	if (it == blockZones.end()) {
		auto& blockBuildings = block->GetBuildings();
		auto bit = blockBuildings.find(token);
		if (bit == blockBuildings.end()) return nullptr;
		Building* building = bit->second;
		if (!building) return nullptr;
		string roomId;
		iss >> roomId;
		for (auto& room : building->GetRooms()) {
			if (room->GetNumber() == roomId)return room;
		}
		return nullptr;
	}
	else {
		Zone* zone = it->second;
		if (!zone) return nullptr;
		string building;
		iss >> building;
		auto& zoneBuildings = zone->GetBuildings();
		auto bit = zoneBuildings.find(building);
		if (bit == zoneBuildings.end()) return nullptr;
		Building* bld = bit->second;
		if (!bld) return nullptr;
		string roomId;
		iss >> roomId;
		for (auto& room : bld->GetRooms()) {
			if (room->GetNumber() == roomId)return room;
		}
		return nullptr;
	}
}

bool Map::CheckXY(int x, int y) const {
	if (x < 0 || y < 0 || x >= width || y >= height) {
		return false;
	}
	return true;
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
			if (zone) acreageUsed += zone->GetAcreage();
		}
		for (const auto& [name, building] : buildings) {
			if (building) acreageUsed += building->GetAcreage();
		}
		float acreageRemain = acreageTotal - acreageUsed;

		bool acreageAllocate = false;
		if (acreageRemain > 0) {
			for (auto& [name, building] : buildings) {
				if (!building) continue;
				float acreageTmp = building->GetAcreage();
				float acreageMax = building->GetAcreageMax();
				float acreageMin = building->GetAcreageMin();

				float acreageExpand = acreageMax - acreageTmp;

				if (acreageExpand > acreageRemain && acreageRemain > 0) {
					float acreageNew = acreageTmp + acreageRemain;
					if (acreageNew >= acreageMin && acreageNew <= acreageMax) {
						building->SetAcreage(acreageNew);
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
			if (zone) elements.push_back(zone);
		}
		for (const auto& [name, building] : buildings) {
			if (building) elements.push_back(building);
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
			class Space : public Quad {
			public:
				Space(Quad* r1, Quad* r2) : r1(r1), r2(r2) { acreage = r1->GetAcreage() + r2->GetAcreage(); }
				Quad* r1, * r2;
			};
			while (elements.size() > 2) {
				Space* tmp = new Space(elements[elements.size() - 1], elements[elements.size() - 2]);
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
				if (auto chunk = dynamic_cast<Space*>(tmp)) {
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
						if (dynamic_cast<Space*>(rect1)) elements.push_back(rect1);
						if (dynamic_cast<Space*>(rect2)) elements.push_back(rect2);
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

void Map::ClearZero() {
	auto blocks = roadnet->GetBlocks();
	for (auto block : blocks) {
		if (!block) continue;
		for (auto it = block->GetZones().begin(); it != block->GetZones().end(); ) {
			Zone* zone = it->second;
			if (zone != nullptr && zone->GetAcreage() == 0) {
				delete zone;
				zones.erase(it->first);
				it = block->GetZones().erase(it);
			}
			else {
				++it;
			}
		}
		for (auto it = block->GetBuildings().begin(); it != block->GetBuildings().end(); ) {
			Building* building = it->second;
			if (building != nullptr && building->GetAcreage() == 0) {
				delete building;
				buildings.erase(it->first);
				it = block->GetBuildings().erase(it);
			}
			else {
				++it;
			}
		}
	}
}

