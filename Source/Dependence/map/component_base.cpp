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

void ComponentFactory::RegisterComponent(const string& id, function<Component* ()> creator) {
    registries[id] = creator;
}

Component* ComponentFactory::CreateComponent(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool ComponentFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void ComponentFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

