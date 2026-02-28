#include "vehicle_base.h"


using namespace std;

void VehicleFactory::RegisterVehicle(const string& id, function<Vehicle* ()> creator) {
    registries[id] = creator;
}

Vehicle* VehicleFactory::CreateVehicle(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool VehicleFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void VehicleFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}
