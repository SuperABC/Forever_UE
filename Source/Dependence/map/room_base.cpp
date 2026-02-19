#include "room_base.h"


using namespace std;

void Room::SetParent(Building* building) {
    parentBuilding = building;
}

void Room::SetParent(Component* component) {
    parentComponent = component;
}

Building* Room::GetParentBuilding() const {
    return parentBuilding;
}

Component* Room::GetParentComponent() const {
    return parentComponent;
}

int Room::GetOwner() const {
    return ownerId;
}

void Room::SetOwner(int id) {
    this->ownerId = id;
}

bool Room::GetStateOwned() const {
    return stateOwned;
}

void Room::SetStateOwned(bool state) {
    this->stateOwned = state;
}

int Room::GetLayer() const {
    return layer;
}

void Room::SetLayer(int layer) {
    this->layer = layer;
}

int Room::GetFace() const {
    return face;
}

void Room::SetFace(int face) {
    this->face = face;
}

void RoomFactory::RegisterRoom(const string& id, function<Room* ()> creator) {
    registries[id] = creator;
}

Room* RoomFactory::CreateRoom(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;
    
    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool RoomFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void RoomFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

