#include "vehicle_mod.h"


using namespace std;

VehicleMod::VehicleMod() {

}

VehicleMod::~VehicleMod() {

}

void VehicleFactory::RegisterVehicle(const string& id,
	function<VehicleMod* ()> creator, function<void(VehicleMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void VehicleFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

VehicleMod* VehicleFactory::CreateVehicle(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Vehicle %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Vehicle %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Vehicle " + id + " creater is null.\n");
	}

	return nullptr;
}

bool VehicleFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void VehicleFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void VehicleFactory::DestroyVehicle(VehicleMod* Vehicle) const {
	if (!Vehicle) {
		debugf("Warning: Vehicle is null when deleting.\n");
		return;
	}

	auto it = registries.find(Vehicle->GetType());
	if (it == registries.end()) {
		debugf("Warning: Vehicle %s not registered when deleting.\n", Vehicle->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Vehicle);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Vehicle " + string(Vehicle->GetType()) + " deleter is null.\n");
	}
}

