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

    return 0;
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


