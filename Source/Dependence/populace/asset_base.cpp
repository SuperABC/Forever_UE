#include "asset_base.h"


using namespace std;

void AssetFactory::RegisterAsset(const string& id, function<Asset* ()> creator) {
    registries[id] = creator;
}

Asset* AssetFactory::CreateAsset(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;
    
    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool AssetFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void AssetFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}
