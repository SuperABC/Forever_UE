#include "station_base.h"
#include "../common/error.h"
#include "../common/utility.h"

using namespace std;

void Station::AddSlot(const glm::vec3 position, const glm::vec3 direction) {
    slots.push_back({position, direction});
}

pair<glm::vec3, glm::vec3> Station::GetSlot(int index) const {
    if (index < 0 || index >= slots.size()) {
        THROW_EXCEPTION(OutOfRangeException, "Station slot index out of bounds: " + to_string(index) + ".\n");
    }
    return slots[index];
}

vector<pair<glm::vec3, glm::vec3>> Station::GetSlots() const {
    return slots;
}

void StationFactory::RegisterStation(const string& id,
    function<Station*()> creator, function<void(Station*)> deleter) {
    registries[id] = {creator, deleter};
}

Station* StationFactory::CreateStation(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool StationFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void StationFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

void StationFactory::DestroyStation(Station* station) const {
    if (!station) return;
    auto it = registries.find(station->GetType());
    if (it != registries.end()) {
        it->second.second(station);
    } else {
        debugf(("Deleter not found for " + station->GetType() + ".\n").data());
    }
}