#include "../common/error.h"
#include "../common/utility.h"

#include "asset_base.h"


using namespace std;

Asset::Asset() :
	asset() {
	// 全成员默认构造

}

Asset::~Asset() {
	// 无析构

}

const string& Asset::GetAsset() const {
	// 获取资产名称
	return asset;
}

void Asset::SetAsset(const string& asset) {
	// 设置资产名称
	this->asset = asset;
}

void AssetFactory::RegisterAsset(const string& id,
	function<Asset* ()> creator, function<void(Asset*)> deleter) {
	// 注册构造器和析构器
	registries[id] = { creator, deleter };
}

Asset* AssetFactory::CreateAsset(const string& id) {
	// 根据配置构造资产
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		return nullptr;
	}

	auto it = registries.find(id);
	if (it != registries.end()) {
		return it->second.first();
	}
	return nullptr;
}

bool AssetFactory::CheckRegistered(const string& id) const {
	// 检查是否注册
	return registries.find(id) != registries.end();
}

void AssetFactory::SetConfig(const string& name, bool config) {
	// 设置启用配置
	configs[name] = config;
}

void AssetFactory::DestroyAsset(Asset* asset) const {
	// 析构资产
	if (!asset) {
		debugf("Asset is null before destroying.\n");
		return;
	}
	auto it = registries.find(asset->GetType());
	if (it != registries.end()) {
		it->second.second(asset);
	}
	else {
		debugf("Deleter not found for %s.\n", asset->GetType().data());
	}
}