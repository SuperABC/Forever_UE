#include "manufacture_mod.h"


using namespace std;

ManufactureMod::ManufactureMod() :
	targets() {

}

ManufactureMod::~ManufactureMod() {

}

void ManufactureFactory::RegisterManufacture(const string& id,
	function<ManufactureMod* ()> creator, function<void(ManufactureMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void ManufactureFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

ManufactureMod* ManufactureFactory::CreateManufacture(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Manufacture %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Manufacture %s not registered when creating.\n", id.data());
		return nullptr;
	}

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

void ManufactureFactory::DestroyManufacture(ManufactureMod* Manufacture) const {
	if (!Manufacture) {
		debugf("Warning: Manufacture is null when deleting.\n");
		return;
	}

	auto it = registries.find(Manufacture->GetType());
	if (it == registries.end()) {
		debugf("Warning: Manufacture %s not registered when deleting.\n", Manufacture->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Manufacture);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Manufacture " + string(Manufacture->GetType()) + " deleter is null.\n");
	}
}

