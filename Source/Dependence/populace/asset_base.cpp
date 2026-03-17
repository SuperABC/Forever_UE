#include "../common/error.h"
#include "../common/utility.h"

#include "asset_base.h"


using namespace std;

// 全成员默认构造
Asset::Asset() :
	asset() {

}

// 无析构
Asset::~Asset() {

}

// 获取资产名称
const string& Asset::GetAsset() const {
	return asset;
}

// 设置资产名称
void Asset::SetAsset(const string& asset) {
	this->asset = asset;
}

// 清空注册
void AssetFactory::RemoveAll() {
    for(auto &config : configs) {
        config.second = false;
    }
}

// 注册资产
void AssetFactory::RegisterAsset(const string& id,
    function<Asset* ()> creator, function<void(Asset*)> deleter) {
    registries[id] = { creator, deleter };
}

// 创建资产
Asset* AssetFactory::CreateAsset(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Asset %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Asset  %s  not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Asset " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool AssetFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void AssetFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构资产
void AssetFactory::DestroyAsset(Asset* asset) const {
    if (!asset) {
        debugf("Warning: Asset is null when deleting.\n");
        return;
    }

    auto it = registries.find(asset->GetType());
    if (it == registries.end()) {
        debugf("Warning: Asset %s not registered when deleting.\n", asset->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(asset);
    } else {
        THROW_EXCEPTION(NullPointerException, "Asset " + asset->GetType() + " deleter is null.\n");
    }
}
