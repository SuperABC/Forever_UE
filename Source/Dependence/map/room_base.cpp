#include "room_base.h"

#include <sstream>
#include <iomanip>

using namespace std;

Room::Room() {
    parentBuilding = nullptr;
    parentComponent = nullptr;
    storage = nullptr;
}

Room::~Room() {
    if (storage) {
        delete storage;
        storage = nullptr;
    }
    for (auto manufacture : manufactures) {
        delete manufacture;
    }
    manufactures.clear();
}

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

unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> Room::GetDoors() {
    return doors;
}

void Room::SetDoors(unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> doors) {
    this->doors = doors;
}

unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> Room::GetWindows() {
    return windows;
}

void Room::SetWindows(unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> windows) {
    this->windows = windows;
}

void Room::SetAddress(int number) {
    ostringstream oss;
    oss << setw(4) << setfill('0') << number;
    address = oss.str();
}

string Room::GetAddress() const {
    return address;
}

vector<int> Room::GetTenants() {
    return tenants;
}

void Room::AddTenant(int id) {
    tenants.push_back(id);
}

bool Room::RemoveTenant(int id) {
    for (auto& tenant : tenants) {
        if (tenant == id) {
            tenant = tenants.back();
            tenants.pop_back();
            return true;
        }
    }
    return false;
}

vector<int> Room::GetWorkers() {
    return workers;
}

void Room::AddWorker(int id) {
    workers.push_back(id);
}

bool Room::RemoveWorker(int id) {
    for (auto& worker : workers) {
        if (worker == id) {
            worker = workers.back();
            workers.pop_back();
            return true;
        }
    }
    return false;
}

Storage* Room::GetStorage() {
    return storage;
}

void Room::SetStorage(Storage* storage) {
    this->storage = storage;
}

Storage* Room::ClearStorage() {
    auto item = storage;
    storage = nullptr;
    return item;
}

vector<Manufacture*> Room::GetManufactures() {
    return manufactures;
}

void Room::AddManufacture(Manufacture* manufacture) {
    manufactures.push_back(manufacture);
}

Manufacture* Room::RemoveManufacture(string manufacture) {
    Manufacture* item = nullptr;
    for(auto& m : manufactures) {
        if(m->GetName() == manufacture) {
            item = m;
            m = manufactures.back();
            manufactures.pop_back();
            return item;
        }
    }
    return nullptr;
}

vector<Manufacture*> Room::ClearManufactures() {
    auto items = manufactures;
    manufactures.clear();
    return items;
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

void RoomFactory::DestroyRoom(Room* room) {

}
