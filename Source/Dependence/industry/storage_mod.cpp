#include "storage_mod.h"


using namespace std;

StorageMod::StorageMod() :
	catagories(),
	density(1.f),
	accept(true),
	provide(true) {

}

StorageMod::~StorageMod() {

}

void StorageFactory::RegisterStorage(const string& id,
	function<StorageMod* ()> creator, function<void(StorageMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void StorageFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

StorageMod* StorageFactory::CreateStorage(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Storage %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Storage %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Storage " + id + " creater is null.\n");
	}

	return nullptr;
}

bool StorageFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void StorageFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void StorageFactory::DestroyStorage(StorageMod* Storage) const {
	if (!Storage) {
		debugf("Warning: Storage is null when deleting.\n");
		return;
	}

	auto it = registries.find(Storage->GetType());
	if (it == registries.end()) {
		debugf("Warning: Storage %s not registered when deleting.\n", Storage->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Storage);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Storage " + string(Storage->GetType()) + " deleter is null.\n");
	}
}

