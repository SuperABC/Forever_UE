#include "../common/error.h"
#include "../common/utility.h"

#include "vehicle_base.h"


using namespace std;

// 无构造
Vehicle::Vehicle() {

}

// 无析构
Vehicle::~Vehicle() {
    
}

// 清空注册
void VehicleFactory::RemoveAll() {
    for(auto &config : configs) {
        config.second = false;
    }
}

// 注册载具
void VehicleFactory::RegisterVehicle(const string& id,
    function<Vehicle*()> creator, function<void(Vehicle*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建载具
Vehicle* VehicleFactory::CreateVehicle(const string& id) const {
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
    } else {
        THROW_EXCEPTION(NullPointerException, "Vehicle " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool VehicleFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void VehicleFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析转载具
void VehicleFactory::DestroyVehicle(Vehicle* vehicle) const {
    if (!vehicle) {
        debugf("Warning: Vehicle is null when deleting.\n");
        return;
    }

    auto it = registries.find(vehicle->GetType());
    if (it == registries.end()) {
        debugf("Warning: Vehicle %s not registered when deleting.\n", vehicle->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(vehicle);
    } else {
        THROW_EXCEPTION(NullPointerException, "Vehicle " + vehicle->GetType() + " deleter is null.\n");
    }
}

