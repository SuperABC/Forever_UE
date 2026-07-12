#include "asset_mod.h"


using namespace std;

AssetMod::AssetMod() :
	asset(),
	mobility(ASSET_ESTATE),
	weight(-1.f),
	size(-1.f), 
	volume(-1.f),
	backpack(false),
	icon(),
	mesh() {

}

AssetMod::~AssetMod() {

}

void AssetFactory::RegisterAsset(const string& id,
	function<AssetMod* ()> creator, function<void(AssetMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void AssetFactory::RemoveAll() {
	// 禁用所有已注册的资产类型
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

AssetMod* AssetFactory::CreateAsset(const string& id) const {
	auto config = configs.find(id);
	// 检查资产类型是否已启用
	if (config == configs.end() || !config->second) {
		debugf("Warning: Asset %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	// 检查资产类型是否已注册
	if (it == registries.end()) {
		debugf("Warning: Asset %s not registered when creating.\n", id.data());
		return nullptr;
	}

	// 调用注册的构造函数
	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Asset " + id + " creator is null.\n");
	}

	return nullptr;
}

bool AssetFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void AssetFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void AssetMod::DefineEstate() {
	mobility = ASSET_ESTATE;
}

void AssetMod::DefineVehicle() {
	mobility = ASSET_VEHICLE;
}

void AssetMod::DefineObject(float weight, float size, bool backpack) {
	mobility = ASSET_OBJECT;
	this->weight = weight;
	this->size = size;
	this->backpack = backpack;
}

void AssetMod::DefineContainer(float weight, float size, float volume, bool backpack) {
	mobility = ASSET_OBJECT;
	this->weight = weight;
	this->size = size;
	this->volume = volume;
	this->backpack = backpack;
}

void AssetMod::DefineContainee(float weight, float size) {
	mobility = ASSET_CONTAINEE;
	this->weight = weight;
	this->size = size;
}

void AssetFactory::DestroyAsset(AssetMod* assetMod) const {
	// 检查待析构对象是否为空
	if (!assetMod) {
		debugf("Warning: Asset is null when deleting.\n");
		return;
	}

	auto it = registries.find(assetMod->GetType());
	// 检查资产类型是否已注册
	if (it == registries.end()) {
		debugf("Warning: Asset %s not registered when deleting.\n", assetMod->GetType());
		return;
	}

	// 调用注册的析构函数
	if (it->second.second) {
		it->second.second(assetMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Asset " + string(assetMod->GetType()) + " deleter is null.\n");
	}
}

