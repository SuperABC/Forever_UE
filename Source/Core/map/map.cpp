#include "map.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <algorithm>


using namespace std;

TerrainFactory* Map::terrainFactory = nullptr;
RoadnetFactory* Map::roadnetFactory = nullptr;

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
    height(0) {
    if (!terrainFactory) {
        terrainFactory = new TerrainFactory();
    }
    if (!roadnetFactory) {
        roadnetFactory = new RoadnetFactory();
    }
    //if (!zoneFactory) {
    //    zoneFactory = new ZoneFactory();
    //}
    //if (!buildingFactory) {
    //    buildingFactory = new BuildingFactory();
    //}
    //if (!componentFactory) {
    //    componentFactory = new ComponentFactory();
    //}
    //if (!roomFactory) {
    //    roomFactory = new RoomFactory();
    //}
}

Map::~Map() {
    delete roadnet;
}

void Map::LoadConfigs() const {
    terrainFactory->RemoveAll();
    roadnetFactory->RemoveAll();
    //zoneFactory->RemoveAll();
    //buildingFactory->RemoveAll();
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
    //auto zones = Config::GetEnables("zone");
    //for (auto zone : zones) {
    //    zoneFactory->SetConfig(zone, true);
    //}
    //auto buildings = Config::GetEnables("building");
    //for (auto building : buildings) {
    //    buildingFactory->SetConfig(building, true);
    //}
    //auto components = Config::GetEnables("component");
    //for (auto component : components) {
    //    componentFactory->SetConfig(component, true);
    //}
    //auto rooms = Config::GetEnables("room");
    //for (auto room : rooms) {
    //    roomFactory->SetConfig(room, true);
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

int Map::Init(int blockX, int blockY) {
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

    return 0;
}

void Map::Destroy() {
    blocks.clear();
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

