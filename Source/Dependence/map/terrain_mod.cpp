#include "terrain_mod.h"


using namespace std;

// 无构造
TerrainMod::TerrainMod() {

}

// 无析构
TerrainMod::~TerrainMod() {

}

// 注册地形
void TerrainFactory::RegisterTerrain(const string& id,
    function<TerrainMod* ()> creator, function<void(TerrainMod*)> deleter) {
    registries[id] = { creator, deleter };
}

// 清空注册
void TerrainFactory::RemoveAll() {
    for (auto& config : configs) {
        config.second = false;
    }
}

// 创建地形
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

// 检查是否注册
bool TerrainFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void TerrainFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 获取所有启用地形
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

// 析构地形
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

