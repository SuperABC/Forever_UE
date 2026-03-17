#include "map.h"
#include "utility.h"
#include "error.h"
#include "json.h"
#include "config.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>


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

Block::Block(int x, int y) : offsetX(x), offsetY(y) {
    elements = vector<vector<shared_ptr<Element>>>(BLOCK_SIZE,
        vector<shared_ptr<Element>>(BLOCK_SIZE, nullptr));
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            elements[j][i] = make_shared<Element>();
        }
    }
}

Block::~Block() {

}

string Block::GetTerrain(int x, int y) const {
    if (!CheckXY(x, y)) {
        return "";
    }
    return elements[y - offsetY][x - offsetX]->GetTerrain();
}

bool Block::SetTerrain(int x, int y, const string& terrain, float height) {
    if (!CheckXY(x, y)) {
        return false;
    }
    return elements[y - offsetY][x - offsetX]->SetTerrain(terrain, height);
}

float Block::GetHeight(int x, int y) const {
    if (!CheckXY(x, y)) {
        debugf("Warning: Invalid block coordinates (%d, %d) for block (%d, %d).\n", x, y, offsetX, offsetY);
        return 0.f;
    }
    return elements[y - offsetY][x - offsetX]->GetHeight();
}

bool Block::CheckXY(int x, int y) const {
    if (x < offsetX) return false;
    if (y < offsetY) return false;
    if (x >= offsetX + BLOCK_SIZE) return false;
    if (y >= offsetY + BLOCK_SIZE) return false;
    return true;
}

shared_ptr<Element> Block::GetElement(int x, int y) const {
    if (!CheckXY(x, y)) {
        return nullptr;
    }
    return elements[y - offsetY][x - offsetX];
}

Map::Map() :
    width(0),
    height(0),
    playerPos(0.f, 0.f),
    roadnet(nullptr),
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

    if (roadnet) {
        for (auto plot : roadnet->GetPlots()) {
            if (!plot) continue;
            for (auto& [name, zone] : plot->GetZones()) {
                if (!zone) continue;
                for (auto& [name, building] : zone->GetBuildings()) {
                    if (!building) continue;
                    for (auto room : building->GetRooms()) {
                        if (room) roomFactory->DestroyRoom(room);
                    }
                    for (auto component : building->GetComponents()) {
                        if (component) componentFactory->DestroyComponent(component);
                    }
                    if (building) buildingFactory->DestroyBuilding(building);
                }
                if (zone) zoneFactory->DestroyZone(zone);
            }
            for (auto& [name, building] : plot->GetBuildings()) {
                if (!building) continue;
                for (auto room : building->GetRooms()) {
                    if (room) roomFactory->DestroyRoom(room);
                }
                for (auto component : building->GetComponents()) {
                    if (component) componentFactory->DestroyComponent(component);
                }
                if (building) buildingFactory->DestroyBuilding(building);
            }
        }
        roadnetFactory->DestroyRoadnet(roadnet);
        roadnet = nullptr;
    }
}

void Map::InitTerrains(unordered_map<string, HMODULE>& modHandles,
    vector<string>& dlls) {
    terrainFactory->RegisterTerrain(OceanTerrain::GetId(),
        []() { return new OceanTerrain(); },
        [](Terrain* terrain) { delete terrain; }
    );
    terrainFactory->RegisterTerrain(MountainTerrain::GetId(),
        []() { return new MountainTerrain(); },
        [](Terrain* terrain) { delete terrain; }
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
    roadnetFactory->RegisterRoadnet(JingRoadnet::GetId(),
        []() { return new JingRoadnet(); },
        [](Roadnet* roadnet) { delete roadnet; });

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
    zoneFactory->RegisterZone(DefaultZone::GetId(), DefaultZone::ZoneGenerator,
        []() { return new DefaultZone(); },
        [](Zone* zone) { delete zone; }
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
    buildingFactory->RegisterBuilding(DefaultResidentialBuilding::GetId(), DefaultResidentialBuilding::GetPower(),
        []() { return new DefaultResidentialBuilding(); },
        [](Building* building) { delete building; }
    );
    buildingFactory->RegisterBuilding(DefaultWorkingBuilding::GetId(), DefaultWorkingBuilding::GetPower(),
        []() { return new DefaultWorkingBuilding(); },
        [](Building* building) { delete building; }
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
    componentFactory->RegisterComponent(DefaultResidentialComponent::GetId(),
        []() { return new DefaultResidentialComponent(); },
        [](Component* component) { delete component; }
    );
    componentFactory->RegisterComponent(DefaultWorkingComponent::GetId(),
        []() { return new DefaultWorkingComponent(); },
        [](Component* component) { delete component; }
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
    roomFactory->RegisterRoom(DefaultResidentialRoom::GetId(),
        []() { return new DefaultResidentialRoom(); },
        [](Room* room) { delete room; }
    );
    roomFactory->RegisterRoom(DefaultWorkingRoom::GetId(),
        []() { return new DefaultWorkingRoom(); },
        [](Room* room) { delete room; }
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

int Map::Init(int blockX, int blockY, Traffic* traffic) {
    Destroy();

    if (blockX < 1 || blockY < 1) {
        THROW_EXCEPTION(InvalidArgumentException, "Invalid map size.\n");
    }

    width = blockX * BLOCK_SIZE;
    height = blockY * BLOCK_SIZE;
    playerPos.first = width / 2.f;
    playerPos.second = height / 2.f;

    debugf("Log: Initializing map with size %d x %d (block size: %d x %d).\n", width, height, blockX, blockY);
    blocks = vector<vector<shared_ptr<Block>>>(blockY,
        vector<shared_ptr<Block>>(blockX, nullptr));
    for (int i = 0; i < blockX; ++i) {
        for (int j = 0; j < blockY; ++j) {
            blocks[j][i] = make_shared<Block>(i * BLOCK_SIZE, j * BLOCK_SIZE);
        }
    }

    debugf("Log: Generate terrains.\n");
    auto getTerrain = [this](int x, int y) -> string {
        return this->GetTerrain(x, y);
        };
    auto getHeight = [this](int x, int y) -> float {
        return this->GetHeight(x, y);
        };
    auto setElement = [this](int x, int y, const string terrain, float height) -> bool {
        return this->SetTerrain(x, y, terrain, height);
        };
    auto terrains = terrainFactory->GetTerrains();
    sort(terrains.begin(), terrains.end(),
        [](const Terrain* a, const Terrain* b) {
            return a->GetPriority() > b->GetPriority();
        });
    for (auto& terrain : terrains) {
        terrain->DistributeTerrain(width, height, setElement, getTerrain, getHeight);
    }
    terrainFactory->DestroyTerrains(terrains);

    debugf("Log: Generate roadnet.\n");
    roadnet = roadnetFactory->GetRoadnet();
    if (!roadnet) {
        THROW_EXCEPTION(RuntimeException, "No enabled roadnet in config.\n");
    }
    roadnet->DistributeRoadnet(width, height, getTerrain, traffic->GetStationFactory(), traffic->GetRouteFactory());
    roadnet->AllocateAddress();

    debugf("Log: Generate zones.\n");
    auto zoneTypes = zoneFactory->GetTypes();
    for (auto plot : roadnet->GetPlots()) {
        if (!plot) continue;
        vector<Zone*> zones;
        for (auto type : zoneTypes) {
            for (auto zone : zoneFactory->CreateZones(type, plot)) {
                zones.push_back(zone);
            }
        }
        for (auto zone : zones) {
            zone->SetZone(plot, buildingFactory);
            string name = zone->GetName();
            plot->AddZone(name, zone);
        }
    }
    for (auto plot : roadnet->GetPlots()) {
        if (!plot) continue;
        auto zs = plot->GetZones();
        for (auto& z : zs) {
            Zone* zone = z.second;
            if (!zone) continue;
            zone->SetParent(plot);
            for (auto& b : zone->GetBuildings()) {
                Building* building = b.second;
                if (!building) continue;
                building->SetParent(zone);
                building->SetParent(plot);
            }
            if (this->zones.find(z.first) != this->zones.end()) {
                THROW_EXCEPTION(RuntimeException, "Duplicate zone name: " + z.first + ".\n");
            }
            this->zones[z.first] = zone;
        }
    }

    debugf("Log: Generate buildings.\n");
    auto uniques = buildingFactory->GetNums(roadnet->GetPlots());
    auto powers = buildingFactory->GetPowers();
    vector<vector<pair<string, float>>> cdfs(AREA_GREEN);
    for (int area = 1; area <= AREA_GREEN; ++area) {
        float sum = 0.f;
        for (auto& power : powers) {
            sum += power.second[area - 1];
            cdfs[area - 1].emplace_back(power.first, sum);
        }
        if (sum == 0.f) {
            cdfs[area - 1].clear();
            continue;
        }
        for (auto& cdf : cdfs[area - 1]) {
            cdf.second /= sum;
        }
    }
    for (auto plot : roadnet->GetPlots()) {
        if (!plot) continue;
        float acreagePlot = plot->GetAcreage();
        for (auto& zone : plot->GetZones()) {
            if (zone.second) acreagePlot -= zone.second->GetAcreage();
        }
        if (acreagePlot <= 0.f) continue;

        float acreageTmp = 0.f;
        int attempt = 0;
        while (acreageTmp < acreagePlot) {
            if (attempt > 16) break;

            Building* building = nullptr;

            if (uniques[plot].size() > 0) {
                auto name = uniques[plot].back();
                uniques[plot].pop_back();
                building = buildingFactory->CreateBuilding(name);
            }
            else {
                float rand = GetRandom(int(1e5)) / 1e5f;
                for (auto& cdf : cdfs[plot->GetArea()]) {
                    if (rand < cdf.second) {
                        building = buildingFactory->CreateBuilding(cdf.first);
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
            if (acreagePlot - acreageTmp < acreageMin) {
                ++attempt;
                continue;
            }
            else if (acreagePlot - acreageTmp < acreageBuilding) {
                acreageBuilding = acreagePlot - acreageTmp;
            }

            acreageTmp += acreageBuilding;
            building->SetAcreage(acreageBuilding);
            building->SetParent(plot);
            plot->AddBuilding(building->GetName(), building);
            if (buildings.find(building->GetName()) != buildings.end()) {
                THROW_EXCEPTION(RuntimeException, "Duplicate building name: " + building->GetName() + ".\n");
            }
            buildings[building->GetName()] = building;
        }
    }

    ArrangePlots();
    for (auto plot : roadnet->GetPlots()) {
        if (!plot) continue;
        auto zones = plot->GetZones();
        for (auto& [name, zone] : zones) {
            if (!zone) continue;
            zone->ArrangeBuildings();
            SetZone(zone, name);
            for (auto& building : zone->GetBuildings()) {
                if (!building.second) continue;
                SetBuilding(building.second, building.first, make_pair(zone->GetLeft(), zone->GetBottom()));
            }
        }
        auto buildings = plot->GetBuildings();
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
        building->FinishInit();
        building->LayoutRooms(componentFactory, roomFactory, layout);
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
            building->FinishInit();
            building->LayoutRooms(componentFactory, roomFactory, layout);
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

    return capacity / 4;
}

void Map::Checkin(vector<Person*> citizens, Time* time, AssetFactory* factory) const {
    // 筛选成年市民
    vector<Person*> adults;
    adults.reserve(citizens.size());
    for (auto citizen : citizens) {
        if (citizen && citizen->GetAge(time) >= 18) { // 空指针检查
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
            zone->SetOwner(index);
            auto asset = factory->CreateAsset("zone");
            if (asset) {
                asset->SetAsset(zone->GetAddress());
                adults[index]->AddAsset(asset);
            }
            for (auto& buildingPair : zone->GetBuildings()) {
                Building* building = buildingPair.second;
                if (!building) continue;
                building->SetOwner(index);
                for (auto room : building->GetRooms()) {
                    if (!room) continue;
                    room->SetOwner(index);
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
                    building->SetOwner(index);
                    auto asset = factory->CreateAsset("building");
                    if (asset) {
                        asset->SetAsset(building->GetAddress());
                        adults[index]->AddAsset(asset);
                    }
                    for (auto room : building->GetRooms()) {
                        if (!room) continue;
                        room->SetOwner(index);
                        if (room->IsResidential()) {
                            residences.emplace_back(room, 0);
                        }
                    }
                }
                else {
                    for (auto room : building->GetRooms()) {
                        if (!room) continue;
                        int index = GetRandom(int(adults.size()));
                        room->SetOwner(index);
                        auto asset = factory->CreateAsset("room");
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
            building->SetOwner(index);
            auto asset = factory->CreateAsset("building");
            if (asset) {
                asset->SetAsset(building->GetAddress());
                adults[index]->AddAsset(asset);
            }
            for (auto room : building->GetRooms()) {
                if (!room) continue;
                room->SetOwner(index);
                if (room->IsResidential()) {
                    residences.emplace_back(room, 0);
                }
            }
        }
        else {
            for (auto room : building->GetRooms()) {
                if (!room) continue;
                int index = GetRandom(int(adults.size()));
                room->SetOwner(index);
                auto asset = factory->CreateAsset("room");
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
        if (adult->GetSpouse() && adult->GetSpouse()->GetHome() == nullptr) {
            if (GetRandom(10) > 0) {
                adult->GetSpouse()->SetHome(room);
                ++num;
                for (auto child : adult->GetChilds()) {
                    if (child && child->GetAge(time) < 18) {
                        child->SetHome(room);
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
    blocks.clear();
}

void Map::Tick(int day, int hour, int min, int sec) {

}

void Map::Print() const {

}

void Map::Load(const string& path) {

}

void Map::Save(const string& path) const {

}

void Map::ApplyChange(Change* change, Story* story,
    vector<function<pair<bool, ValueType>(const string&)>>& getValues) {

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

shared_ptr<Block> Map::GetBlock(int x, int y) const {
    if (!CheckXY(x, y)) {
        THROW_EXCEPTION(InvalidArgumentException, "Invalid block query position.\n");
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    if (blockY < 0 || blockY >= (int)blocks.size() || blockX < 0 || blockX >= (int)blocks[0].size()) {
        THROW_EXCEPTION(OutOfRangeException, "Block index out of range.\n");
    }
    return blocks[blockY][blockX];
}

shared_ptr<Element> Map::GetElement(int x, int y) const {
    if (!CheckXY(x, y)) {
        THROW_EXCEPTION(InvalidArgumentException, "Invalid element query position.\n");
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    if (blockY < 0 || blockY >= (int)blocks.size() || blockX < 0 || blockX >= (int)blocks[0].size()) {
        THROW_EXCEPTION(OutOfRangeException, "Block index out of range.\n");
    }
    auto block = blocks[blockY][blockX];
    if (!block) {
        THROW_EXCEPTION(NullPointerException, "Block is null.\n");
    }
    return block->GetElement(x, y);
}

string Map::GetTerrain(int x, int y) const {
    if (!CheckXY(x, y)) {
        return "";
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    if (blockY >= (int)blocks.size() || blockX >= (int)blocks[0].size()) {
        return "";
    }
    auto block = blocks[blockY][blockX];
    if (!block) return "";
    return block->GetTerrain(x, y);
}

bool Map::SetTerrain(int x, int y, const string& terrain, float height) {
    if (!CheckXY(x, y)) {
        return false;
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    if (blockY >= (int)blocks.size() || blockX >= (int)blocks[0].size()) {
        return false;
    }
    auto block = blocks[blockY][blockX];
    if (!block) return false;
    return block->SetTerrain(x, y, terrain, height);
}

float Map::GetHeight(int x, int y) const {
    if (!CheckXY(x, y)) {
        debugf("Warning: Invalid map coordinates (%d, %d).\n", x, y);
        return 0.f;
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    if (blockY >= (int)blocks.size() || blockX >= (int)blocks[0].size()) {
        debugf("Warning: Block index out of range (%d, %d).\n", blockX, blockY);
        return 0.f;
    }
    auto block = blocks[blockY][blockX];
    if (!block) {
        debugf("Warning: Block is null at (%d, %d).\n", blockX, blockY);
        return 0.f;
    }
    return block->GetHeight(x, y);
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

Zone* Map::GetZone(const string& name) const {
    auto it = zones.find(name);
    if (it == zones.end()) return nullptr;
    return it->second;
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

void Map::SetZone(Zone* zone, const string& name) {
    if (!zone) {
        THROW_EXCEPTION(NullPointerException, "Zone is null.\n");
    }
    auto plot = zone->GetParent();
    if (!plot) {
        THROW_EXCEPTION(NullPointerException, "Zone's parent plot is null.\n");
    }

    // 计算四个顶点（世界坐标）
    auto v1 = plot->GetPosition(zone->GetPosX() + zone->GetSizeX() / 2.f, zone->GetPosY() + zone->GetSizeY() / 2.f);
    auto v2 = plot->GetPosition(zone->GetPosX() - zone->GetSizeX() / 2.f, zone->GetPosY() + zone->GetSizeY() / 2.f);
    auto v3 = plot->GetPosition(zone->GetPosX() - zone->GetSizeX() / 2.f, zone->GetPosY() - zone->GetSizeY() / 2.f);
    auto v4 = plot->GetPosition(zone->GetPosX() + zone->GetSizeX() / 2.f, zone->GetPosY() - zone->GetSizeY() / 2.f);

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
    auto plot = building->GetParentPlot();
    if (!plot) {
        THROW_EXCEPTION(NullPointerException, "Building's parent plot is null.\n");
    }

    auto v1 = plot->GetPosition(building->GetPosX() + building->GetSizeX() / 2.f, building->GetPosY() + building->GetSizeY() / 2.f);
    auto v2 = plot->GetPosition(building->GetPosX() - building->GetSizeX() / 2.f, building->GetPosY() + building->GetSizeY() / 2.f);
    auto v3 = plot->GetPosition(building->GetPosX() - building->GetSizeX() / 2.f, building->GetPosY() - building->GetSizeY() / 2.f);
    auto v4 = plot->GetPosition(building->GetPosX() + building->GetSizeX() / 2.f, building->GetPosY() - building->GetSizeY() / 2.f);

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
    auto plot = building->GetParentPlot();
    if (!plot) {
        THROW_EXCEPTION(NullPointerException, "Building's parent plot is null.\n");
    }

    auto v1 = plot->GetPosition(offset.first + building->GetPosX() + building->GetSizeX() / 2.f, offset.second + building->GetPosY() + building->GetSizeY() / 2.f);
    auto v2 = plot->GetPosition(offset.first + building->GetPosX() - building->GetSizeX() / 2.f, offset.second + building->GetPosY() + building->GetSizeY() / 2.f);
    auto v3 = plot->GetPosition(offset.first + building->GetPosX() - building->GetSizeX() / 2.f, offset.second + building->GetPosY() - building->GetSizeY() / 2.f);
    auto v4 = plot->GetPosition(offset.first + building->GetPosX() + building->GetSizeX() / 2.f, offset.second + building->GetPosY() - building->GetSizeY() / 2.f);

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

Plot* Map::LocatePlot(const string& address) const {
    istringstream iss(address);
    string road;
    int id = -1;
    iss >> road >> id;
    if (id < 0 || !roadnet) return nullptr;
    return roadnet->LocatePlot(road, id);
}

Zone* Map::LocateZone(const string& address) const {
    istringstream iss(address);
    string road;
    int id = -1;
    iss >> road >> id;
    if (id < 0 || !roadnet) return nullptr;
    auto plot = roadnet->LocatePlot(road, id);
    if (!plot) return nullptr;

    string zone;
    iss >> zone;
    auto& plotZones = plot->GetZones();
    auto it = plotZones.find(zone);
    if (it == plotZones.end()) return nullptr;
    return it->second;
}

Building* Map::LocateBuilding(const string& address) const {
    istringstream iss(address);
    string road;
    int id = -1;
    iss >> road >> id;
    if (id < 0 || !roadnet) return nullptr;
    auto plot = roadnet->LocatePlot(road, id);
    if (!plot) return nullptr;

    string token;
    iss >> token;
    auto& plotZones = plot->GetZones();
    auto it = plotZones.find(token);
    if (it == plotZones.end()) {
        auto& plotBuildings = plot->GetBuildings();
        auto bit = plotBuildings.find(token);
        if (bit == plotBuildings.end()) return nullptr;
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
    auto plot = roadnet->LocatePlot(road, id);
    if (!plot) return nullptr;

    string token;
    iss >> token;
    auto& plotZones = plot->GetZones();
    auto it = plotZones.find(token);
    if (it == plotZones.end()) {
        auto& plotBuildings = plot->GetBuildings();
        auto bit = plotBuildings.find(token);
        if (bit == plotBuildings.end()) return nullptr;
        Building* building = bit->second;
        if (!building) return nullptr;
        int roomId = -1;
        iss >> roomId;
        if (roomId < 0 || roomId >= (int)building->GetRooms().size()) return nullptr;
        return building->GetRooms()[roomId];
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
        int roomId = -1;
        iss >> roomId;
        if (roomId < 0 || roomId >= (int)bld->GetRooms().size()) return nullptr;
        return bld->GetRooms()[roomId];
    }
}

void Map::ArrangePlots() {
    if (!roadnet) return;
    auto plots = roadnet->GetPlots();
    for (auto plot : plots) {
        if (!plot) continue;
        auto zones = plot->GetZones();
        auto buildings = plot->GetBuildings();

        if (zones.empty() && buildings.empty()) continue;

        float acreageTotal = plot->GetAcreage();
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
            emptyRect = new Plot();
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

        Quad container = Quad(plot->GetSizeX() / 2, plot->GetSizeY() / 2, plot->GetSizeX(), plot->GetSizeY());
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