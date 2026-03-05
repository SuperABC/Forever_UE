#include "component_base.h"
#include "building_base.h"


using namespace std;

void Component::SetParent(Building* building) {
    parentBuilding = building;
}

Building* Component::GetParent() const {
    return parentBuilding;
}

vector<Room*>& Component::GetRooms() {
    return rooms;
}

void Component::AddRoom(Room* room) {
    rooms.push_back(room);
}

void ComponentFactory::RegisterComponent(const string& id,
    function<Component* ()> creator, function<void(Component*)> deleter) {
    registries[id] = { creator, deleter };
}

Component* ComponentFactory::CreateComponent(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool ComponentFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void ComponentFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

void ComponentFactory::DestroyComponent(Component* component) const {
    auto it = registries.find(component->GetType());
    if (it != registries.end()) {
        return it->second.second(component);
    }
    else{
        THROW_EXCEPTION(StructureCrashException, "Deleter not found for " + component->GetType() + ".\n");
    }
}
