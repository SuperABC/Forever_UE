#include "map.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

TerrainFactory* Map::terrainFactory = nullptr;
RoadnetFactory* Map::roadnetFactory = nullptr;
ZoneFactory* Map::zoneFactory = nullptr;
BuildingFactory* Map::buildingFactory = nullptr;
ComponentFactory* Map::componentFactory = nullptr;
RoomFactory* Map::roomFactory = nullptr;

Element::Element() {

}

Element::~Element() {

}

string Element::GetTerrain() const {
    return terrain;
}

bool Element::SetTerrain(string terrain) {
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
    return this->zone;
}

bool Element::SetZone(string zone) {
    this->zone = zone;

    return true;
}

string Element::GetBuilding() const {
    return this->building;
}

bool Element::SetBuilding(string building) {
    this->building = building;

    return true;
}

Block::Block(int x, int y) : offsetX(x), offsetY(y) {
    elements = vector<vector<shared_ptr<Element>>>(BLOCK_SIZE,
        vector<shared_ptr<Element>>(BLOCK_SIZE, nullptr));

    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            elements[j][i] = make_shared<Element>();
        }
    }
}

Block::~Block() {

}

string Block::GetTerrain(int x, int y) const {
    if (!CheckXY(x, y))
        return "";

    return elements[y - offsetY][x - offsetX]->GetTerrain();
}

bool Block::SetTerrain(int x, int y, string terrain) {
    if (!CheckXY(x, y)) {
        return false;
    }

    return elements[y - offsetY][x - offsetX]->SetTerrain(terrain);
}

bool Block::CheckXY(int x, int y) const {
    if (x < offsetX)return false;
    if (y < offsetY)return false;
    if (x >= offsetX + BLOCK_SIZE)return false;
    if (y >= offsetY + BLOCK_SIZE)return false;
    return true;
}

shared_ptr<Element> Block::GetElement(int x, int y) {
    if (CheckXY(x, y))
        return elements[y - offsetY][x - offsetX];
    else
        return nullptr;
}

Map::Map() {
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
         delete roadnet;
     }
}

void Map::SetResourcePath(string path) {
    resourcePath = path;
}

void Map::InitTerrains(unordered_map<string, HMODULE>& modHandles) {
    terrainFactory->RegisterTerrain(OceanTerrain::GetId(),
        []() { return new OceanTerrain(); });
    terrainFactory->RegisterTerrain(MountainTerrain::GetId(),
        []() { return new MountainTerrain(); });

    return;
    
	string modPath = "Mod.dll";
    HMODULE modHandle;
    if(modHandles.find(modPath) != modHandles.end()) {
        modHandle = modHandles[modPath];
    }
    else {
        modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
    }
    if (modHandle) {
        debugf("Mod dll loaded successfully.\n");

        RegisterModTerrainsFunc registerFunc = (RegisterModTerrainsFunc)GetProcAddress(modHandle, "RegisterModTerrains");
        if (registerFunc) {
            registerFunc(terrainFactory);
        }
        else {
            debugf("Incorrect dll content.\n");
        }
    }
    else {
        debugf("Failed to load mod.dll.\n");
    }

#ifdef MOD_TEST
    auto terrainList = { "mod" };
    for (const auto& terrainId : terrainList) {
        if (terrainFactory->CheckRegistered(terrainId)) {
            auto terrain = terrainFactory->CreateTerrain(terrainId);
            debugf(("Created terrain: " + terrain->GetName() + " (ID: " + terrainId + ").\n").data());
            delete terrain;
        }
        else {
            debugf("Terrain not registered: %s.\n", terrainId);
        }
    }
#endif // MOD_TEST

}

void Map::InitRoadnets(unordered_map<string, HMODULE>& modHandles) {
    roadnetFactory->RegisterRoadnet(JingRoadnet::GetId(),
        []() { return new JingRoadnet(); });

    return;

    string modPath = "Mod.dll";
    HMODULE modHandle;
    if (modHandles.find(modPath) != modHandles.end()) {
        modHandle = modHandles[modPath];
    }
    else {
        modHandle = LoadLibraryA(modPath.data());
        modHandles[modPath] = modHandle;
    }
    if (modHandle) {
        debugf("Mod dll loaded successfully.\n");

        RegisterModRoadnetsFunc registerFunc = (RegisterModRoadnetsFunc)GetProcAddress(modHandle, "RegisterModRoadnets");
        if (registerFunc) {
            registerFunc(roadnetFactory);
        }
        else {
            debugf("Incorrect dll content.\n");
        }
    }
    else {
        debugf("Failed to load mod.dll.\n");
    }

#ifdef MOD_TEST
    auto roadnetList = { "mod" };
    for (const auto& roadnetId : roadnetList) {
        if (roadnetFactory->CheckRegistered(roadnetId)) {
            auto roadnet = roadnetFactory->CreateRoadnet(roadnetId);
            debugf("Created roadnet: mod.\n");
            delete roadnet;
        }
        else {
            debugf("Roadnet not registered: %s.\n", roadnetId);
        }
    }
#endif // MOD_TEST

}

void Map::InitZones(unordered_map<string, HMODULE>& modHandles) {
    zoneFactory->RegisterZone(DefaultZone::GetId(),
        []() { return new DefaultZone(); }, DefaultZone::ZoneGenerator);

    return;

    string modPath = "Mod.dll";
    HMODULE modHandle;
    if (modHandles.find(modPath) != modHandles.end()) {
        modHandle = modHandles[modPath];
    }
    else {
        modHandle = LoadLibraryA(modPath.data());
        modHandles[modPath] = modHandle;
    }
    if (modHandle) {
        debugf("Mod dll loaded successfully.\n");

        RegisterModZonesFunc registerFunc = (RegisterModZonesFunc)GetProcAddress(modHandle, "RegisterModZones");
        if (registerFunc) {
            registerFunc(zoneFactory);
        }
        else {
            debugf("Incorrect dll content.\n");
        }
    }
    else {
        debugf("Failed to load mod.dll.\n");
    }

#ifdef MOD_TEST
    auto zoneList = { "mod" };
    for (const auto& zoneId : zoneList) {
        if (zoneFactory->CheckRegistered(zoneId)) {
            auto zone = zoneFactory->CreateZone(zoneId);
            debugf(("Created zone: " + zone->GetName() + " (ID: " + zoneId + ").\n").data());
            delete zone;
        }
        else {
            debugf("Zone not registered: %s.\n", zoneId);
        }
    }
#endif // MOD_TEST

}

void Map::InitBuildings(unordered_map<string, HMODULE>& modHandles) {
    buildingFactory->RegisterBuilding(DefaultResidentialBuilding::GetId(),
        []() { return new DefaultResidentialBuilding(); }, DefaultResidentialBuilding::GetPower());
    buildingFactory->RegisterBuilding(DefaultWorkingBuilding::GetId(),
        []() { return new DefaultWorkingBuilding(); }, DefaultWorkingBuilding::GetPower());

    return;

    string modPath = "Mod.dll";
    HMODULE modHandle;
    if (modHandles.find(modPath) != modHandles.end()) {
        modHandle = modHandles[modPath];
    }
    else {
        modHandle = LoadLibraryA(modPath.data());
        modHandles[modPath] = modHandle;
    }
    if (modHandle) {
        debugf("Mod dll loaded successfully.\n");

        RegisterModBuildingsFunc registerFunc = (RegisterModBuildingsFunc)GetProcAddress(modHandle, "RegisterModBuildings");
        if (registerFunc) {
            registerFunc(buildingFactory);
        }
        else {
            debugf("Incorrect dll content.\n");
        }
    }
    else {
        debugf("Failed to load mod.dll.\n");
    }

#ifdef MOD_TEST
    auto buildingList = { "mod" };
    for (const auto& buildingId : buildingList) {
        if (buildingFactory->CheckRegistered(buildingId)) {
            auto building = buildingFactory->CreateBuilding(buildingId);
            debugf(("Created building: " + building->GetName() + " (ID: " + buildingId + ").\n").data());
			delete building;
        }
        else {
            debugf("Building not registered: %s.\n", buildingId);
        }
    }
#endif // MOD_TEST

}

void Map::InitComponents(unordered_map<string, HMODULE>& modHandles) {
    componentFactory->RegisterComponent(DefaultResidentialComponent::GetId(),
        []() { return new DefaultResidentialComponent(); });
    componentFactory->RegisterComponent(DefaultWorkingComponent::GetId(),
        []() { return new DefaultWorkingComponent(); });

    return;

    string modPath = "Mod.dll";
    HMODULE modHandle;
    if (modHandles.find(modPath) != modHandles.end()) {
        modHandle = modHandles[modPath];
    }
    else {
        modHandle = LoadLibraryA(modPath.data());
        modHandles[modPath] = modHandle;
    }
    if (modHandle) {
        debugf("Mod dll loaded successfully.\n");
        RegisterModComponentsFunc registerFunc = (RegisterModComponentsFunc)GetProcAddress(modHandle, "RegisterModComponents");
        if (registerFunc) {
            registerFunc(componentFactory);
        }
        else {
            debugf("Incorrect dll content.\n");
        }
    }
    else {
        debugf("Failed to load mod.dll.\n");
    }

#ifdef MOD_TEST
    auto componentList = { "mod" };
    for (const auto& componentId : componentList) {
        if (componentFactory->CheckRegistered(componentId)) {
            auto component = componentFactory->CreateComponent(componentId);
            debugf(("Created component: " + component->GetName() + " (ID: " + componentId + ").\n").data());
            delete component;
        }
        else {
            debugf("Component not registered: %s.\n", componentId);
        }
	}
#endif // MOD_TEST

}

void Map::InitRooms(unordered_map<string, HMODULE>& modHandles) {
    roomFactory->RegisterRoom(DefaultResidentialRoom::GetId(),
        []() { return new DefaultResidentialRoom(); });
    roomFactory->RegisterRoom(DefaultWorkingRoom::GetId(),
        []() { return new DefaultWorkingRoom(); });

    return;

    string modPath = "Mod.dll";
    HMODULE modHandle;
    if (modHandles.find(modPath) != modHandles.end()) {
        modHandle = modHandles[modPath];
    }
    else {
        modHandle = LoadLibraryA(modPath.data());
        modHandles[modPath] = modHandle;
    }
    if (modHandle) {
        debugf("Mod dll loaded successfully.\n");
        RegisterModRoomsFunc registerFunc = (RegisterModRoomsFunc)GetProcAddress(modHandle, "RegisterModRooms");
        if (registerFunc) {
            registerFunc(roomFactory);
        }
        else {
            debugf("Incorrect dll content.\n");
        }
    }
    else {
        debugf("Failed to load mod.dll.\n");
    }

#ifdef MOD_TEST
    auto roomList = { "mod" };
    for (const auto& roomId : roomList) {
        if (roomFactory->CheckRegistered(roomId)) {
            auto room = roomFactory->CreateRoom(roomId);
            debugf(("Created room: " + room->GetName() + " (ID: " + roomId + ").\n").data());
            delete room;
        }
        else {
            debugf("Room not registered: %s.\n", roomId);
        }
    }
#endif // MOD_TEST

}

void Map::ReadConfigs(string path) const {
	path = resourcePath + path;
    if (!filesystem::exists(path)) {
        THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
    }

    JsonReader reader;
    JsonValue root;

    ifstream fin(path);
    if (!fin.is_open()) {
        THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
    }
    if (reader.Parse(fin, root)) {
        for (auto terrain : root["mods"]["terrain"]) {
            terrainFactory->SetConfig(terrain.AsString(), true);
        }
        roadnetFactory->SetConfig(root["mods"]["roadnet"].AsString(), true);
        for (auto zone : root["mods"]["zone"]) {
            zoneFactory->SetConfig(zone.AsString(), true);
        }
        for (auto building : root["mods"]["building"]) {
            buildingFactory->SetConfig(building.AsString(), true);
        }
        for (auto component : root["mods"]["component"]) {
            componentFactory->SetConfig(component.AsString(), true);
        }
        for (auto room : root["mods"]["room"]) {
            roomFactory->SetConfig(room.AsString(), true);
        }
    }
    else {
        fin.close();
        THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
    }
    fin.close();
}

int Map::Init(int blockX, int blockY) {
    // 清除已有内容
    Destroy();

    // 地图尺寸需要为正
    if (blockX < 1 || blockY < 1) {
        THROW_EXCEPTION(InvalidArgumentException, "Invalid map size.\n");
        return 0;
    }

    // 计算地图实际长宽
    width = blockX * BLOCK_SIZE;
    height = blockY * BLOCK_SIZE;

    // 构建区块
	debugf("Initializing map with size %d x %d (block size: %d x %d).\n", width, height, blockX, blockY);
    blocks = vector<vector<shared_ptr<Block>>>(blockY,
        vector<shared_ptr<Block>>(blockX, nullptr));
    for (int i = 0; i < blockX; i++) {
        for (int j = 0; j < blockY; j++) {
            blocks[j][i] = make_shared<Block>(i * BLOCK_SIZE, j * BLOCK_SIZE);
        }
    }

    // 随机生成地形
	debugf("Generate terrains.\n");
    auto getTerrain = [this](int x, int y) -> string {
        return this->GetTerrain(x, y);
        };
    auto setTerrain = [this](int x, int y, const string terrain) -> bool {
        return this->SetTerrain(x, y, terrain);
        };
    auto terrains = terrainFactory->GetTerrains();
    sort(terrains.begin(), terrains.end(),
        [](const Terrain* a, const Terrain* b) {
            return a->GetPriority() > b->GetPriority();
        });
    for (auto& terrain : terrains) {
        terrain->DistributeTerrain(width, height, setTerrain, getTerrain);
    }
    for(auto &terrain : terrains) {
        delete terrain;
	}

    // 随机生成路网
	debugf("Generate roadnet.\n");
    roadnet = roadnetFactory->GetRoadnet();
    if (!roadnet) {
        THROW_EXCEPTION(InvalidConfigException, "No enabled roadnet in config.\n");
    }
    roadnet->DistributeRoadnet(width, height, getTerrain);
    roadnet->AllocateAddress();

    // 随机生成园区
    debugf("Generate zones.\n");
    zoneFactory->GenerateAll(roadnet->GetPlots(), buildingFactory);
    for (auto plot : roadnet->GetPlots()) {
        auto zs = plot->GetZones();
        for (auto z : zs) {
            z.second->SetParent(plot);
            for (auto b : z.second->GetBuildings()) {
                b.second->SetParent(z.second);
                b.second->SetParent(plot);
            }
            if (zones.find(z.first) != zones.end()) {
                THROW_EXCEPTION(InvalidConfigException, "Duplicate zone name: " + z.first + ".\n");
            }
            zones[z.first] = z.second;
        }
    }

    // 随机生成建筑
    debugf("Generate buildings.\n");
    auto powers = buildingFactory->GetPowers();
    vector<vector<pair<string, float>>> cdfs(AREA_GREEN);
    for (int area = 1; area <= AREA_GREEN; area++) {
        float sum = 0.f;
        for (auto power : powers) {
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
        float acreagePlot = plot->GetAcreage();
        for (auto zone : plot->GetZones()) {
            acreagePlot -= zone.second->GetAcreage();
        }
        if (acreagePlot <= 0.f)continue;

        float acreageTmp = 0.f;
        int attempt = 0;
        while (acreageTmp < acreagePlot) {
            if (attempt > 16)break;

            Building* building = nullptr;

            float rand = GetRandom(int(1e5)) / 1e5f;
            for (auto cdf : cdfs[plot->GetArea()]) {
                if (rand < cdf.second) {
                    building = buildingFactory->CreateBuilding(cdf.first);
                    break;
                }
            }

            if (!building) {
                attempt++;
                continue;
            }

            float acreageBuilding = building->RandomAcreage();
            float acreageMin = building->GetAcreageMin();
            float acreageMax = building->GetAcreageMax();
            if (acreagePlot - acreageTmp < acreageMin) {
                attempt++;
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
                THROW_EXCEPTION(InvalidConfigException, "Duplicate building name: " + building->GetName() + ".\n");
            }
            buildings[building->GetName()] = building;
        }
    }

    // 随机分布建筑与园区
    ArrangePlots();
    for (auto plot : roadnet->GetPlots()) {
        auto zones = plot->GetZones();
        for (auto zone : zones) {
            zone.second->ArrangeBuildings();
        }
    }

    // 随机生成组合与房间
    debugf("Generate components and rooms.\n");
	int capacity = 0;
    if(layout)delete layout;
    layout = Building::ReadTemplates(resourcePath + "layouts/");
    for (auto &building : buildings) {
        building.second->FinishInit();
        building.second->LayoutRooms(componentFactory, roomFactory, layout);
        for (auto component : building.second->GetComponents()) {
            component->SetParent(building.second);
            for (auto room : component->GetRooms()) {
                room->SetParent(component);
                room->SetParent(building.second);
	        capacity += room->GetLivingCapacity();
            }
        }
    }
    for (auto zone : zones) {
        for (auto& building : zone.second->GetBuildings()) {
            building.second->FinishInit();
            building.second->LayoutRooms(componentFactory, roomFactory, layout);
            for (auto component : building.second->GetComponents()) {
                component->SetParent(building.second);
                for (auto room : component->GetRooms()) {
                    room->SetParent(component);
                    room->SetParent(building.second);
			 	    capacity += room->GetLivingCapacity();
                }
            }
        }
    }

    return capacity / 4;
}

void Map::Checkin(vector<Person*> citizens, Time* time) const {
    // 筛选成年市民
    auto adults = vector<Person*>();
    for (auto citizen : citizens) {
        if (citizen->GetAge(time) < 18)continue;
        adults.push_back(citizen);
    }
    if (adults.size() == 0)return;

    // 为房产分配房东
    auto residences = vector<pair<Room*, int>>();
    for (auto zone : zones) {
        // 政府园区
        if (zone.second->GetStateOwned()) {
            for (auto building : zone.second->GetBuildings()) {
                building.second->SetStateOwned(true);
                for (auto room : building.second->GetRooms()) {
                    room->SetStateOwned(true);
                    if (room->IsResidential()) {
                        residences.push_back({ room, 0 });
                    }
                }
            }
            continue;
        }

        // 私人单人园区
        if (GetRandom(100) < 2) {
            int index = GetRandom(int(adults.size()));
            zone.second->SetOwner(index);
            citizens[index]->AddAsset(new ZoneAsset(zone.second));
            for (auto building : zone.second->GetBuildings()) {
                building.second->SetOwner(index);
                for (auto room : building.second->GetRooms()) {
                    room->SetOwner(index);
                    if (room->IsResidential()) {
                        residences.push_back({ room, 0 });
                    }
                }
            }
        }
        // 私人混合园区
        else {
            for (auto building : zone.second->GetBuildings()) {
                // 私人单人建筑
                if (GetRandom(100) < 5) {
                    int index = GetRandom(int(adults.size()));
                    building.second->SetOwner(index);
                    citizens[index]->AddAsset(new BuildingAsset(building.second));
                    for (auto room : building.second->GetRooms()) {
                        room->SetOwner(index);
                        if (room->IsResidential()) {
                            residences.push_back({ room, 0 });
                        }
                    }
                }
                // 私人混合建筑
                else {
                    for (auto room : building.second->GetRooms()) {
                        int index = GetRandom(int(adults.size()));
                        room->SetOwner(index);
                        citizens[index]->AddAsset(new RoomAsset(room));
                        if (room->IsResidential()) {
                            residences.push_back({ room, 0 });
                        }
                    }
                }
            }
        }
    }
    for (auto building : buildings) {
        // 政府建筑
        if (building.second->GetStateOwned()) {
            for (auto room : building.second->GetRooms()) {
                room->SetStateOwned(true);
                if (room->IsResidential()) {
                    residences.push_back({ room, 0 });
                }
            }
            continue;
        }

        // 私人单人建筑
        if (GetRandom(100) < 5) {
            int index = GetRandom(int(adults.size()));
            building.second->SetOwner(index);
            citizens[index]->AddAsset(new BuildingAsset(building.second));
            for (auto room : building.second->GetRooms()) {
                room->SetOwner(index);
                if (room->IsResidential()) {
                    residences.push_back({ room, 0 });
                }
            }
        }
        // 私人混合建筑
        else {
            for (auto room : building.second->GetRooms()) {
                int index = GetRandom(int(adults.size()));
                room->SetOwner(index);
                citizens[index]->AddAsset(new RoomAsset(room));
                if (room->IsResidential()) {
                    residences.push_back({ room, 0 });
                }
            }
        }
    }

    // 分配市民住所
    for (auto adult : adults) {
        if (adult->GetHome())continue;
        if (residences.size() <= 0)break;

        int index = GetRandom((int)residences.size());
        auto& residence = residences[index];

        int num = 1;
        adult->SetHome(residence.first);
        if (adult->GetSpouse() && adult->GetSpouse()->GetHome() == nullptr) {
            if (GetRandom(10) > 0) {
                adult->GetSpouse()->SetHome(residence.first);
                num++;
                for (auto child : adult->GetChilds()) {
                    if (child->GetAge(time) < 18) {
                        child->SetHome(residence.first);
                        num++;
                    }
                }
            }
        }

        if (num >= residence.second) {
            residences[index] = residences.back();
            residences.pop_back();
        }
    }
}

void Map::Destroy() {
    blocks.clear();
}

void Map::Tick(int day, int hour, int min, int sec) {

}

void Map::Print() const {

}

void Map::Load(string path) {

}

void Map::Save(string path) const {

}

void Map::ApplyChange(Change* change, Story* story,
    vector<function<pair<bool, ValueType>(const string&)>>& getValues) {

}

pair<int, int> Map::GetSize() const {
    return make_pair(width, height);
}

bool Map::CheckXY(int x, int y) const {
    if (x < 0)return false;
    if (y < 0)return false;
    if (x >= width)return false;
    if (y >= height)return false;
    return true;
}

shared_ptr<Block> Map::GetBlock(int x, int y) const {
    if (!CheckXY(x, y)) {
        THROW_EXCEPTION(InvalidArgumentException, "invalid block query position.\n");
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    return blocks[blockY][blockX];
}

shared_ptr<Element> Map::GetElement(int x, int y) const {
    if (!CheckXY(x, y)) {
        THROW_EXCEPTION(InvalidArgumentException, "invalid block query position.\n");
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    return blocks[blockY][blockX]->GetElement(x, y);
}

string Map::GetTerrain(int x, int y) const {
    if (!CheckXY(x, y)) {
        return "";
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    if (blockY >= blocks.size() || blockX >= blocks[0].size()) {
        return "";
    }

    return blocks[blockY][blockX]->GetTerrain(x, y);
}

bool Map::SetTerrain(int x, int y, string terrain) {
    if (!CheckXY(x, y)) {
        return false;
    }

    int blockX = x / BLOCK_SIZE;
    int blockY = y / BLOCK_SIZE;

    if (blockY >= blocks.size() || blockX >= blocks[0].size()) {
        return false;
    }

    return blocks[blockY][blockX]->SetTerrain(x, y, terrain);
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
    for (const auto& zone : zones) {
        for (auto building : zone.second->GetBuildings()) {
            const auto& current = building.second->GetComponents();
            components.insert(components.end(), current.begin(), current.end());
        }
    }
    for (const auto& building : buildings) {
        const auto& current = building.second->GetComponents();
        components.insert(components.end(), current.begin(), current.end());
    }
    return components;
}

vector<Room*> Map::GetRooms() const {
    vector<Room*> rooms;
    for (const auto& zone : zones) {
        for (auto building : zone.second->GetBuildings()) {
            const auto& current = building.second->GetRooms();
            rooms.insert(rooms.end(), current.begin(), current.end());
        }
    }
    for (const auto& building : buildings) {
        const auto& current = building.second->GetRooms();
        rooms.insert(rooms.end(), current.begin(), current.end());
    }
    return rooms;
}

void Map::ArrangePlots() {
    auto plots = roadnet->GetPlots();
    for (auto plot : plots) {
        auto zones = plot->GetZones();
        auto buildings = plot->GetBuildings();

        if (zones.empty() && buildings.empty()) continue;

        float acreageTotal = plot->GetAcreage();
        float acreageUsed = 0.f;

        for (const auto& zone : zones) {
            acreageUsed += zone.second->GetAcreage();
        }
        for (const auto& building : buildings) {
            acreageUsed += building.second->GetAcreage();
        }
        float acreageRemain = acreageTotal - acreageUsed;

        bool acreageAllocate = false;
        if (acreageRemain > 0) {
            for (auto& building : buildings) {
                float acreageTmp = building.second->GetAcreage();
                float acreageMax = building.second->GetAcreageMax();
                float acreageMin = building.second->GetAcreageMin();

                float acreageExpand = acreageMax - acreageTmp;

                if (acreageExpand > acreageRemain && acreageRemain > 0) {
                    float acreageNew = acreageTmp + acreageRemain;
                    if (acreageNew >= acreageMin && acreageNew <= acreageMax) {
                        building.second->SetAcreage(acreageNew);
                        acreageUsed += acreageRemain;
                        acreageRemain = 0.f;
                        acreageAllocate = true;
                        break;
                    }
                }
            }
        }

		Quad* emptyRect = nullptr;
        vector<Quad *> elements;
        if (acreageRemain > 0 && !acreageAllocate) {
            emptyRect = new Plot();
            emptyRect->SetAcreage(acreageRemain);
            elements.push_back(emptyRect);
        }

        for (const auto& zone : zones) {
            elements.push_back(zone.second);
        }
        for (const auto& building : buildings) {
            elements.push_back(building.second);
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
                Quad *r1, *r2;
            };
            while (elements.size() > 2) {
                Chunk* tmp = new Chunk(elements[elements.size() - 1], elements[elements.size() - 2]);
                elements.pop_back();
                int i = (int)elements.size() - 2;
                for (; i >= 0; i--) {
                    if (tmp->GetAcreage() > elements[i]->GetAcreage()) {
                        elements[i + 1] = elements[i];
                    }
                    else {
                        elements[i + 1] = tmp;
                        break;
                    }
                }
                if (i < 0)elements[0] = tmp;
            }

            if (container.GetSizeX() > container.GetSizeY()) {
                if (GetRandom(2)) {
                    int divX = int(container.GetLeft() +
                        (container.GetRight() - container.GetLeft()) * elements[0]->GetAcreage() / container.GetAcreage());
                    if (abs(divX - container.GetLeft()) < 2)divX = (int)container.GetLeft();
                    if (abs(divX - container.GetRight()) < 2)divX = (int)container.GetRight();
                    elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
                    elements[1]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
                }
                else {
                    int divX = int(container.GetLeft() +
                        (container.GetRight() - container.GetLeft()) * elements[1]->GetAcreage() / container.GetAcreage());
                    if (abs(divX - container.GetLeft()) < 2)divX = (int)container.GetLeft();
                    if (abs(divX - container.GetRight()) < 2)divX = (int)container.GetRight();
                    elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), (float)divX, container.GetTop());
                    elements[0]->SetVertices((float)divX, container.GetBottom(), container.GetRight(), container.GetTop());
                }
            }
            else {
                if (GetRandom(2)) {
                    int divY = int(container.GetBottom() +
                        (container.GetTop() - container.GetBottom()) * elements[0]->GetAcreage() / container.GetAcreage());
                    if (abs(divY - container.GetBottom()) < 2)divY = (int)container.GetBottom();
                    if (abs(divY - container.GetTop()) < 2)divY = (int)container.GetTop();
                    elements[0]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
                    elements[1]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
                }
                else {
                    int divY = int(container.GetBottom() +
                        (container.GetTop() - container.GetBottom()) * elements[1]->GetAcreage() / container.GetAcreage());
                    if (abs(divY - container.GetBottom()) < 2)divY = (int)container.GetBottom();
                    if (abs(divY - container.GetTop()) < 2)divY = (int)container.GetTop();
                    elements[1]->SetVertices(container.GetLeft(), container.GetBottom(), container.GetRight(), (float)divY);
                    elements[0]->SetVertices(container.GetLeft(), (float)divY, container.GetRight(), container.GetTop());
                }
            }

            while (elements.size() > 0) {
                auto tmp = elements.back();
                elements.pop_back();
                if (auto chunk = dynamic_cast<Chunk *>(tmp)) {
                    Quad* rect1 = chunk->r1;
                    Quad* rect2 = chunk->r2;

                    if (tmp->GetAcreage() > 0) {
                        if (tmp->GetSizeX() > tmp->GetSizeY()) {
                            if (GetRandom(2)) {
                                int divX = int(tmp->GetLeft() +
                                    tmp->GetSizeX() * rect1->GetAcreage() / tmp->GetAcreage());
                                if (abs(divX - tmp->GetLeft()) < 2)divX = (int)tmp->GetLeft();
                                if (abs(divX - tmp->GetRight()) < 2)divX = (int)tmp->GetRight();
                                rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
                                rect2->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
                            }
                            else {
                                int divX = int(tmp->GetLeft() +
                                    tmp->GetSizeX() * rect2->GetAcreage() / tmp->GetAcreage());
                                if (abs(divX - tmp->GetLeft()) < 2)divX = (int)tmp->GetLeft();
                                if (abs(divX - tmp->GetRight()) < 2)divX = (int)tmp->GetRight();
                                rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), (float)divX, tmp->GetTop());
                                rect1->SetVertices((float)divX, tmp->GetBottom(), tmp->GetRight(), tmp->GetTop());
                            }
                        }
                        else {
                            if (GetRandom(2)) {
                                int divY = int(tmp->GetBottom() +
                                    tmp->GetSizeY() * rect1->GetAcreage() / tmp->GetAcreage());
                                if (abs(divY - tmp->GetBottom()) < 2)divY = (int)tmp->GetBottom();
                                if (abs(divY - tmp->GetTop()) < 2)divY = (int)tmp->GetTop();
                                rect1->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
                                rect2->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
                            }
                            else {
                                int divY = int(tmp->GetBottom() +
                                    tmp->GetSizeY() * rect2->GetAcreage() / tmp->GetAcreage());
                                if (abs(divY - tmp->GetBottom()) < 2)divY = (int)tmp->GetBottom();
                                if (abs(divY - tmp->GetTop()) < 2)divY = (int)tmp->GetTop();
                                rect2->SetVertices(tmp->GetLeft(), tmp->GetBottom(), tmp->GetRight(), (float)divY);
                                rect1->SetVertices(tmp->GetLeft(), (float)divY, tmp->GetRight(), tmp->GetTop());
                            }
                        }
                        if (dynamic_cast<Chunk *>(rect1))elements.push_back(rect1);
                        if (dynamic_cast<Chunk *>(rect2))elements.push_back(rect2);
                    }
                    delete chunk;
                }
            }
        }

        if(emptyRect) {
            delete emptyRect;
		}
    }
}


