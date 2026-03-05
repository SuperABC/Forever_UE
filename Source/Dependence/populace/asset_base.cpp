#include "asset_base.h"

#include "../common/error.h"
#include "../common/utility.h"


using namespace std;

void AssetFactory::RegisterAsset(const string& id,
    function<Asset* ()> creator, function<void(Asset*)> deleter) {
    registries[id] = { creator, deleter };
}

Asset* AssetFactory::CreateAsset(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;
    
    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool AssetFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void AssetFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

void AssetFactory::DestroyAsset(Asset* asset) const {
    if(!asset)return;
    auto it = registries.find(asset->GetType());
    if (it != registries.end()) {
        return it->second.second(asset);
    }
    else{
        debugf(("Deleter not found for " + asset->GetType() + ".\n").data());
    }
}
