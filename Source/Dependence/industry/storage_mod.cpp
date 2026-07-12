#include "storage_mod.h"


using namespace std;

StorageMod::StorageMod() :
	categories(),
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
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

StorageMod* StorageFactory::CreateStorage(const string& id) const {
	// 检查仓库是否已启用
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Storage %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	// 检查仓库是否已注册
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Storage %s not registered when creating.\n", id.data());
		return nullptr;
	}

	// 调用注册的创建函数
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

void StorageFactory::DestroyStorage(StorageMod* storageMod) const {
	// 检查模组是否为空
	if (!storageMod) {
		debugf("Warning: Storage is null when deleting.\n");
		return;
	}

	// 检查仓库是否已注册
	auto it = registries.find(storageMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Storage %s not registered when deleting.\n", storageMod->GetType());
		return;
	}

	// 调用注册的析构函数
	if (it->second.second) {
		it->second.second(storageMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Storage " + string(storageMod->GetType()) + " deleter is null.\n");
	}
}

