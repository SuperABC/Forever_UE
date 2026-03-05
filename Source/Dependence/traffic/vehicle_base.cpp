#include "vehicle_base.h"
#include "../common/error.h"
#include "../common/utility.h"

using namespace std;

void VehicleFactory::RegisterVehicle(const string& id,
    function<Vehicle*()> creator, function<void(Vehicle*)> deleter) {
    registries[id] = {creator, deleter};
}

Vehicle* VehicleFactory::CreateVehicle(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool VehicleFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void VehicleFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

void VehicleFactory::DestroyVehicle(Vehicle* vehicle) const {
    if (!vehicle) return;
    auto it = registries.find(vehicle->GetType());
    if (it != registries.end()) {
        it->second.second(vehicle);
    } else {
        debugf(("Deleter not found for " + vehicle->GetType() + ".\n").data());
    }
}