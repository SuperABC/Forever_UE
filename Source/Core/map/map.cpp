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
#include "story/script.h"
#include "story/change.h"
#include "player/player.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <limits>

#undef max


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

pair<bool, float> Element::GetWater() const {
	return water;
}

bool Element::SetTerrain(const string& terrain, const pair<bool, float> water) {
	this->terrain = terrain;
	this->water = water;
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

vector<pair<Quad, float>> Element::GetHatches() const {
	return hatches;
}

void Element::AddHatch(Quad q, float rotation) {
	hatches.emplace_back(q, rotation);
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

pair<bool, float> Chunk::GetWater(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return { false, 0.f };
	}
	return elements[y - offsetY][x - offsetX]->GetWater();
}

bool Chunk::SetTerrain(int x, int y, const string& terrain, const pair<bool, float> water) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return false;
	}
	return elements[y - offsetY][x - offsetX]->SetTerrain(terrain, water);
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
	return elements[y - offsetY][x - offsetX]->SetZone(zone);
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

vector<pair<Quad, float>> Chunk::GetHatches(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return {};
	}
	return elements[y - offsetY][x - offsetX]->GetHatches();
}

bool Chunk::AddHatch(int x, int y, Quad q, float rotation) {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for chunk (%d, %d).\n", x, y, offsetX, offsetY);
		return false;
	}
	elements[y - offsetY][x - offsetX]->AddHatch(q, rotation);
	return true;
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
	terrainTextures(),
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

	terrainTextures = {
		{ "plain", {0, "/Game/Asset/Textures/Terrain/PlainDiffuse.PlainDiffuse"} },
		{ "construction", {0, "/Game/Asset/Textures/Terrain/PlainDiffuse.PlainDiffuse"} }
	};
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

void Map::InitBlocks(int chunkX, int chunkY) {
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
	pair<bool, float> water = { false, 0.f };
	auto getTerrain = [this](int x, int y) -> string {
		return this->GetTerrain(x, y);
		};
	auto setTerrain = [this, &water](int x, int y, string terrain) -> bool {
		return this->SetTerrain(x, y, terrain, water);
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
		terrains.back()->SetupTexture();
	}
	sort(terrains.begin(), terrains.end(),
		[](const Terrain* a, const Terrain* b) {
			return a->GetPriority() > b->GetPriority();
		});
	auto preset = terrainTextures.size();
	for (auto i = 0; i < terrains.size(); i++) {
		terrainTextures[terrains[i]->GetType()] = {i + static_cast<int>(preset), terrains[i]->GetTexture()};
		water = terrains[i]->GetWater();
		terrains[i]->DistributeTerrain(width, height, getTerrain, setTerrain, getHeight, setHeight);
	}
	for (auto &terrain : terrains) {
		delete terrain;
	}
	terrains.clear();

	debugf("Log: Filter plain elements to construction.\n");
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			bool allPlain = true;
			for (int dy = -1; dy <= 1 && allPlain; ++dy) {
				for (int dx = -1; dx <= 1 && allPlain; ++dx) {
					int nx = x + dx, ny = y + dy;
					if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
						allPlain = false;
					} else {
						string t = getTerrain(nx, ny);
						if (t != "plain" && t != "construction") {
							allPlain = false;
						}
					}
				}
			}
			if (allPlain) {
				setTerrain(x, y, "construction");
			}
		}
	}

	debugf("Log: Generate roadnet.\n");
	roadnet = new Roadnet(roadnetFactory, roadnetFactory->GetRoadnet()) ;
	if (!roadnet) {
		THROW_EXCEPTION(RuntimeException, "No enabled roadnet in config.\n");
	}
	roadnet->DistributeRoadnet(width, height, getTerrain);
	roadnet->AllocateAddress();

	debugf("Log: Initializing navigation graph.\n");
	InitNavigationGraph();
}

int Map::InitContents() {
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
	int idx = 0;
	for (auto block : roadnet->GetBlocks()) {
		if (!block) continue;
		float acreageBlock = block->GetAcreage();
		for (auto& zone : block->GetZones()) {
			if (zone.second) acreageBlock -= zone.second->GetAcreage();
		}
		if (acreageBlock <= 0.f) continue;

		vector<string> presets;
		for (auto [type, _] : powers) {
			auto preset = buildingFactory->CreateBuildings(type, block, idx, (int)roadnet->GetBlocks().size());
			presets.insert(presets.end(), preset.begin(), preset.end());
		}
		for (auto& [_, building] : block->GetBuildings()) {
			acreageBlock -= building->GetAcreage();
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
		idx++;
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
			zone->PlacePivots(zone);
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
	auto ApplyHatches = [this](Building* building, float offsetX, float offsetY) {
		auto ground = building->GetFloor(0);
		if (!ground || ground->GetHatches().empty()) return;
		auto block = building->GetParentBlock();
		if (!block) return;
		auto construction = building->GetConstruction();
		float baseX = offsetX + building->GetLeft() + construction.GetLeft();
		float baseY = offsetY + building->GetBottom() + construction.GetBottom();
		float blockRot = block->GetRotation();
		for (auto& hatch : ground->GetHatches()) {
			auto [wx, wy] = block->GetPosition(baseX + hatch.GetPosX(), baseY + hatch.GetPosY());
			Quad q(wx, wy, hatch.GetSizeX(), hatch.GetSizeY());
			AddHatch(q, blockRot);
		}
	};

	for (auto& [_, building] : buildings) {
		if (!building) continue;
		building->PlaceConstruction();
		building->LayoutBuilding(layout);
		ApplyHatches(building, 0.f, 0.f);
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
	for (auto& [_, zone] : zones) {
		if (!zone) continue;
		for (auto& [_, building] : zone->GetBuildings()) {
			if (!building) continue;
			building->PlaceConstruction();
			building->LayoutBuilding(layout);
			ApplyHatches(building, zone->GetLeft(), zone->GetBottom());
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

	for (auto& [_, building] : buildings) {
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

	for (auto connection : navigationConnections) {
		delete connection;
	}
	navigationConnections.clear();
	navigationGraph.clear();
	navigationNodes.clear();

	if(roadnet)delete roadnet;
	roadnet = nullptr;

	if(layout)delete layout;
	layout = nullptr;
}

void Map::Tick(Player* player) {

}

void Map::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {
	auto type = change->GetType();

	auto findCabin = [&](const string& name) -> Cabin* {
		auto sep = name.find(' ');
		if (sep == string::npos) return nullptr;
		string buildingName = name.substr(0, sep);
		string elevatorName = name.substr(sep + 1);
		Building* b = GetBuilding(buildingName);
		if (!b) return nullptr;
		for (auto cabin : b->GetCabins()) {
			auto selfName = cabin->GetScript()->GetValue("self.name");
			if (!selfName.first || !holds_alternative<string>(selfName.second)) continue;
			if (get<string>(selfName.second) == elevatorName) return cabin;
		}
		return nullptr;
	};

	if (type == "add_option") {
		auto obj = dynamic_cast<AddOptionChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to AddOptionChange.\n");
		}
		Condition conditionTarget;
		conditionTarget.ParseCondition(obj->GetName());
		string targetName = ToString(conditionTarget.EvaluateValue(getValues));
		Cabin* cabin = findCabin(targetName);
		if (!cabin) return;
		Condition conditionOption;
		conditionOption.ParseCondition(obj->GetOption());
		string option = ToString(conditionOption.EvaluateValue(getValues));
		cabin->AddOption(option);
	}
	else if (type == "remove_option") {
		auto obj = dynamic_cast<RemoveOptionChange*>(change);
		if (!obj) {
			THROW_EXCEPTION(InvalidArgumentException, "Failed to cast Change to RemoveOptionChange.\n");
		}
		Condition conditionTarget;
		conditionTarget.ParseCondition(obj->GetName());
		string targetName = ToString(conditionTarget.EvaluateValue(getValues));
		Cabin* cabin = findCabin(targetName);
		if (!cabin) return;
		Condition conditionOption;
		conditionOption.ParseCondition(obj->GetOption());
		string option = ToString(conditionOption.EvaluateValue(getValues));
		cabin->RemoveOption(option);
	}
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

pair<bool, float> Map::GetWater(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return { false, 0.f };
	}

	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;

	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return { false, 0.f };
	}
	return chunk->GetWater(x, y);
}

bool Map::SetTerrain(int x, int y, const string& terrain, const std::pair<bool, float> water) {
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
	return chunk->SetTerrain(x, y, terrain, water);
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

vector<pair<Quad, float>> Map::GetHatches(int x, int y) const {
	if (!CheckXY(x, y)) {
		debugf("Warning: Invalid coordinates (%d, %d) for map.\n", x, y);
		return {};
	}
	int chunkX = x / CHUNK_SIZE;
	int chunkY = y / CHUNK_SIZE;
	auto chunk = chunks[chunkY][chunkX];
	if (!chunk) {
		debugf("Warning: Chunk (%d, %d) is null.\n", chunkX, chunkY);
		return {};
	}
	return chunk->GetHatches(x, y);
}

void Map::AddHatch(Quad q, float rotation) {
	float hw = q.GetSizeX() * 0.5f;
	float hh = q.GetSizeY() * 0.5f;
	float ac = std::abs(std::cos(rotation));
	float as_ = std::abs(std::sin(rotation));
	float ahw = hw * ac + hh * as_;
	float ahh = hw * as_ + hh * ac;

	float wx = q.GetPosX(), wy = q.GetPosY();
	int x0 = (int)(wx - ahw);
	int x1 = (int)(wx + ahw);
	int y0 = (int)(wy - ahh);
	int y1 = (int)(wy + ahh);

	for (int cy = y0; cy <= y1; ++cy) {
		for (int cx = x0; cx <= x1; ++cx) {
			if (!CheckXY(cx, cy)) continue;
			auto chunk = chunks[cy / CHUNK_SIZE][cx / CHUNK_SIZE];
			if (!chunk) continue;
			chunk->AddHatch(cx, cy, q, rotation);
		}
	}
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

const unordered_map<string, pair<int, string>>& Map::GetTerrainTextures() const {
	return terrainTextures;
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
		return false;
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

void Map::AddZone(Zone* zone) {
	if (!zone) {
		THROW_EXCEPTION(NullPointerException, "Zone is null.\n");
	}
	if (zones.find(zone->GetName()) != zones.end()) {
		THROW_EXCEPTION(RuntimeException, "Duplicate zone name: " + zone->GetName() + ".\n");
	}
	zones[zone->GetName()] = zone;
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

void Map::AddBuilding(Building* building) {
	if (!building) {
		THROW_EXCEPTION(NullPointerException, "Building is null.\n");
	}
	if (buildings.find(building->GetName()) != buildings.end()) {
		THROW_EXCEPTION(RuntimeException, "Duplicate building name: " + building->GetName() + ".\n");
	}
	buildings[building->GetName()] = building;
}

vector<Component*> Map::GetComponents() const {
	vector<Component*> components;
	for (const auto& [_, zone] : zones) {
		if (!zone) continue;
		for (auto& [_, building] : zone->GetBuildings()) {
			if (!building) continue;
			const auto& current = building->GetComponents();
			components.insert(components.end(), current.begin(), current.end());
		}
	}
	for (const auto& [_, building] : buildings) {
		if (!building) continue;
		const auto& current = building->GetComponents();
		components.insert(components.end(), current.begin(), current.end());
	}
	return components;
}

vector<Room*> Map::GetRooms() const {
	vector<Room*> rooms;
	for (const auto& [_, zone] : zones) {
		if (!zone) continue;
		for (auto& [_, building] : zone->GetBuildings()) {
			if (!building) continue;
			const auto& current = building->GetRooms();
			rooms.insert(rooms.end(), current.begin(), current.end());
		}
	}
	for (const auto& [_, building] : buildings) {
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

vector<Connection*> Map::AutoNavigation(int startIdx, int endIdx) {
	if (navigationNodes.find(startIdx) == navigationNodes.end() ||
		navigationNodes.find(endIdx) == navigationNodes.end()) {
		debugf("Warning: navigation node not found in navigation graph.\n");
		return {};
	}
	if (startIdx == endIdx) return {};

	unordered_map<int, float> dist;
	unordered_map<int, int> prev;
	unordered_map<int, Connection*> prevEdge;

	for (auto& [id, _] : navigationNodes) {
		dist[id] = numeric_limits<float>::max();
	}
	dist[startIdx] = 0.0f;

	using QueueItem = pair<float, int>;
	priority_queue<QueueItem, vector<QueueItem>, greater<QueueItem>> pq;
	pq.push({ 0.0f, startIdx });

	while (!pq.empty()) {
		auto [currentDist, current] = pq.top();
		pq.pop();

		if (currentDist > dist[current]) {
			continue;
		}
		if (current == endIdx) {
			break;
		}

		auto it = navigationGraph.find(current);
		if (it == navigationGraph.end()) continue;

		for (auto& [neighborId, edge] : it->second) {
			float newDist = currentDist + edge->CalcDistance();

			if (newDist < dist[neighborId]) {
				dist[neighborId] = newDist;
				prev[neighborId] = current;
				prevEdge[neighborId] = edge;
				pq.push({ newDist, neighborId });
			}
		}
	}

	if (dist[endIdx] == numeric_limits<float>::max()) {
		debugf("Warning: navigation failed because no connection found in between.\n");
		return {};
	}

	vector<Connection*> path;
	int current = endIdx;
	while (current != startIdx) {
		path.push_back(prevEdge[current]);
		current = prev[current];
	}
	reverse(path.begin(), path.end());

	return path;
}

Connection* Map::AddNode(Node* node, int existingId) {
	if (!node) {
		THROW_EXCEPTION(NullPointerException, "Node is null.\n");
	}
	auto it = navigationNodes.find(existingId);
	if (it == navigationNodes.end()) {
		THROW_EXCEPTION(RuntimeException, "Existing navigation node " + to_string(existingId) + " not found.\n");
	}

	auto connection = new Connection(*it->second, *node);
	navigationGraph[existingId].emplace_back(node->GetId(), connection);
	navigationGraph[node->GetId()].emplace_back(existingId, connection);
	navigationNodes[node->GetId()] = node;
	navigationConnections.push_back(connection);

	return connection;
}

void Map::InitNavigationGraph() {
	navigationNodes.clear();
	navigationGraph.clear();
	for (auto connection : navigationConnections) {
		delete connection;
	}
	navigationConnections.clear();

	for (auto intersection : roadnet->GetIntersections()) {
		navigationNodes[intersection->GetId()] = intersection;
	}
	for (auto road : roadnet->GetRoads()) {
		int v1 = road->GetStart().GetId();
		int v2 = road->GetEnd().GetId();

		navigationGraph[v1].emplace_back(v2, road);
		navigationGraph[v2].emplace_back(v1, road);
	}

	for (auto block : roadnet->GetBlocks()) {
		if (!block) continue;
		for (auto& [road, node] : block->GetRoads()) {
			if (!road || !node) continue;

			AddNode(node, road->GetStart().GetId());
			AddNode(node, road->GetEnd().GetId());
		}
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

		for (const auto& [_, zone] : zones) {
			if (zone) acreageUsed += zone->GetAcreage();
		}
		for (const auto& [_, building] : buildings) {
			if (building) acreageUsed += building->GetAcreage();
		}
		float acreageRemain = acreageTotal - acreageUsed;

		bool acreageAllocate = false;
		if (acreageRemain > 0) {
			for (auto& [_, building] : buildings) {
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

		for (const auto& [_, zone] : zones) {
			if (zone) elements.push_back(zone);
		}
		for (const auto& [_, building] : buildings) {
			if (building) elements.push_back(building);
		}

		if (elements.empty()) continue;

		Quad container(block->GetSizeX() / 2, block->GetSizeY() / 2, block->GetSizeX(), block->GetSizeY());
		container.DivideSpace(elements);

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

