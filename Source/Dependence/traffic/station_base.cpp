#include "../common/error.h"
#include "../common/utility.h"

#include "station_base.h"


using namespace std;

// 全成员默认构造
Station::Station() :
    slots() {

}

// 无析构
Station::~Station() {
    
}

// 获取某一车道
pair<glm::vec3, glm::vec3> Station::GetSlot(int index) const {
    if (index < 0 || index >= slots.size()) {
        THROW_EXCEPTION(OutOfRangeException, "Station slot index out of bounds: " + to_string(index) + ".\n");
    }
    return slots[index];
}

// 获取全部车道
vector<pair<glm::vec3, glm::vec3>> Station::GetSlots() const {
    return slots;
}

// 添加车道
void Station::AddSlot(const glm::vec3& position, const glm::vec3& direction) {
    slots.push_back({position, direction});
}

// 注册车站
void StationFactory::RegisterStation(const string& id,
    function<Station*()> creator, function<void(Station*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建车站
Station* StationFactory::CreateStation(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Station %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Station %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Station " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool StationFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void StationFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构车站
void StationFactory::DestroyStation(Station* station) const {
    if (!station) {
        debugf("Warning: Station is null when deleting.\n");
        return;
    }

    auto it = registries.find(station->GetType());
    if (it == registries.end()) {
        debugf("Warning: Station %s not registered when deleting.\n", station->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(station);
    } else {
        THROW_EXCEPTION(NullPointerException, "Station " + station->GetType() + " deleter is null.\n");
    }
}

