#include "manufacture_mod.h"


using namespace std;

ManufactureMod::ManufactureMod() :
	targets() {

}

ManufactureMod::~ManufactureMod() {

}

ManufactureFactory::ManufactureFactory()
	: registries(),
	configs(),
	temp() {
	temp.registries.reserve(TEMP_RESERVE_CAPACITY);
}

void ManufactureFactory::RegisterManufacture(const string& id,
	function<ManufactureMod* ()> creator, function<void(ManufactureMod*)> deleter) {
	temp.registries[id] = { creator, deleter };
}

void ManufactureFactory::MergeTemp() {
	for (auto& [id, registry] : temp.registries) {
		registries[id] = registry;
	}
}

void ManufactureFactory::CleanTemp() {
	temp.registries.clear();
}

void ManufactureFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

ManufactureMod* ManufactureFactory::CreateManufacture(const string& id) const {
	// 检查工坊是否已启用
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Manufacture %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	// 检查工坊是否已注册
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Manufacture %s not registered when creating.\n", id.data());
		return nullptr;
	}

	// 调用注册的创建函数
	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Manufacture " + id + " creater is null.\n");
	}

	return nullptr;
}

bool ManufactureFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void ManufactureFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void ManufactureFactory::DestroyManufacture(ManufactureMod* manufactureMod) const {
	// 检查模组是否为空
	if (!manufactureMod) {
		debugf("Warning: Manufacture is null when deleting.\n");
		return;
	}

	// 检查工坊是否已注册
	auto it = registries.find(manufactureMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Manufacture %s not registered when deleting.\n", manufactureMod->GetType());
		return;
	}

	// 调用注册的析构函数
	if (it->second.second) {
		it->second.second(manufactureMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Manufacture " + string(manufactureMod->GetType()) + " deleter is null.\n");
	}
}

