#include "asset_mod.h"


using namespace std;

AssetMod::AssetMod() {

}

AssetMod::~AssetMod() {

}

void AssetFactory::RegisterAsset(const string& id,
	function<AssetMod* ()> creator, function<void(AssetMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void AssetFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

AssetMod* AssetFactory::CreateAsset(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Asset %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Asset %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Asset " + id + " creater is null.\n");
	}

	return nullptr;
}

bool AssetFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void AssetFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void AssetFactory::DestroyAsset(AssetMod* assetMod) const {
	if (!assetMod) {
		debugf("Warning: Asset is null when deleting.\n");
		return;
	}

	auto it = registries.find(assetMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Asset %s not registered when deleting.\n", assetMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(assetMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Asset " + string(assetMod->GetType()) + " deleter is null.\n");
	}
}

