#include "vehicle_mod.h"


using namespace std;

VehicleMod::VehicleMod() :
	path(),
	width(0.f),
	length(0.f),
	script({ "empty", {} }) {

}

VehicleMod::~VehicleMod() {

}

void VehicleFactory::RegisterVehicle(const string& id, string label,
	function<VehicleMod* ()> creator, function<void(VehicleMod*)> deleter) {
	registries[id] = { creator, deleter };
	labels[label].push_back(id);
}

void VehicleFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

VehicleMod* VehicleFactory::CreateVehicle(const string& label) const {
	if (labels.find(label) == labels.end()) {
		debugf("Warning: Label %s has no instance.\n", label.data());
		return nullptr;
	}

	string id = labels.find(label)->second[GetRandom((int)labels.find(label)->second.size())];
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

void VehicleFactory::DestroyVehicle(VehicleMod* vehicleMod) const {
	if (!vehicleMod) {
		debugf("Warning: Vehicle is null when deleting.\n");
		return;
	}

	auto it = registries.find(vehicleMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Vehicle %s not registered when deleting.\n", vehicleMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(vehicleMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Vehicle " + string(vehicleMod->GetType()) + " deleter is null.\n");
	}
}

