#include "room.h"

#include "industry/storage.h"
#include "industry/manufacture.h"

#include <sstream>
#include <iomanip>


using namespace std;

Room::Room(RoomFactory* factory, const string& room) :
	mod(factory->CreateRoom(room)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	parentBuilding(nullptr),
	parentComponent(nullptr),
	layer(0),
	doors(),
	windows(),
	number(""),
	stated(false),
	owner(nullptr),
	tenants(),
	workers(),
	storages(),
	manufactures() {

}

Room::~Room() {
	factory->DestroyRoom(mod);

	for(auto &storage : storages) {
		if(storage)delete storage;
		storage = nullptr;
	}
	for(auto &manufacture : manufactures) {
		if(manufacture)delete manufacture;
		manufacture = nullptr;
	}
}

string Room::GetType() const {
	return type;
}

string Room::GetName() const {
	return name;
}

Building* Room::GetParentBuilding() const {
	return parentBuilding;
}

void Room::SetParent(Building* building) {
	parentBuilding = building;
}

Component* Room::GetParentComponent() const {
	return parentComponent;
}

void Room::SetParent(Component* component) {
	parentComponent = component;
}

int Room::GetLayer() const {
	return layer;
}

void Room::SetLayer(int layer) {
	this->layer = layer;
}

const Room::WallHole& Room::GetDoors() const {
	return doors;
}

void Room::SetDoors(const WallHole& doors) {
	this->doors = doors;
}

const Room::WallHole& Room::GetWindows() const {
	return windows;
}

void Room::SetWindows(const WallHole& windows) {
	this->windows = windows;
}

string Room::GetNumber() const {
	return number;
}

void Room::SetNumber(int floor, int number) {
	ostringstream oss;
	if (floor < 0) {
		oss << 'b' << -floor;
	}
	else {
		oss << floor;
	}
	oss << setw(4) << setfill('0') << number;
	this->number = oss.str();
}

void Room::ConfigRoom() {
	mod->ConfigRoom();
}

string Room::GetAddress() const {
	return GetParentBuilding()->GetAddress() + " " + number;
}

pair<float, float> Room::GetPosition(float x, float y) const {
	auto building = GetParentBuilding();
	auto block = GetParentBuilding()->GetParentBlock();
	auto zone = building->GetParentZone();
	if (zone) {
		float blockX = zone->GetPosX() - zone->GetSizeX() / 2.f +
			building->GetPosX() - building->GetSizeX() / 2.f +
			building->GetConstruction().GetPosX() - building->GetConstruction().GetSizeX() / 2.f +
			GetPosX() - GetSizeX() / 2.f + x;
		float blockY = zone->GetPosY() - zone->GetSizeY() / 2.f +
			building->GetPosY() - building->GetSizeY() / 2.f +
			building->GetConstruction().GetPosY() - building->GetConstruction().GetSizeY() / 2.f +
			GetPosY() - GetSizeY() / 2.f + y;
		return block->GetPosition(blockX, blockY);
	}
	else {
		float blockX = building->GetPosX() - building->GetSizeX() / 2.f +
			building->GetConstruction().GetPosX() - building->GetConstruction().GetSizeX() / 2.f +
			GetPosX() - GetSizeX() / 2.f + x;
		float blockY = building->GetPosY() - building->GetSizeY() / 2.f +
			building->GetConstruction().GetPosY() - building->GetConstruction().GetSizeY() / 2.f +
			GetPosY() - GetSizeY() / 2.f + y;
		return block->GetPosition(blockX, blockY);
	}
	return { 0.f, 0.f };
}

bool Room::IsResidential() const {
	return mod->isResidential;
}

bool Room::IsWorkspace() const {
	return mod->isWorkspace;
}

bool Room::IsStorage() const {
	return mod->isStorage;
}

bool Room::IsManufacture() const {
	return mod->isManufacture;
}

int Room::ResidentialCapacity() const {
	return mod->residentialCapacity;
}

int Room::WorkspaceCapacity() const {
	return mod->workspaceCapacity;
}

unordered_map<string, float> Room::StorageConfig() const {
	return mod->storageConfig;
}

vector<string> Room::ManufactureTypes() const {
	return mod->manufactureTypes;
}

bool Room::GetStated() const {
	return stated;
}

void Room::SetStated(bool stated) {
	this->stated = stated;
}

Person* Room::GetOwner() const {
	return owner;
}

void Room::SetOwner(Person* owner) {
	this->owner = owner;
}

const vector<Person*>& Room::GetTenants() const {
	return tenants;
}

void Room::AddTenant(Person* person) {
	tenants.push_back(person);
}

bool Room::RemoveTenant(const string& name) {
	for (auto it = tenants.begin(); it != tenants.end(); ++it) {
		if ((*it)->GetName() == name) {
			tenants.erase(it);
			return true;
		}
	}
	return false;
}

const vector<Person*>& Room::GetWorkers() const {
	return workers;
}

void Room::AddWorker(Person* person) {
	workers.push_back(person);
}

bool Room::RemoveWorker(const string& name) {
	for (auto it = workers.begin(); it != workers.end(); ++it) {
		if ((*it)->GetName() == name) {
			workers.erase(it);
			return true;
		}
	}
	return false;
}

vector<Storage*> Room::GetStorage() const {
	return storages;
}

void Room::AddStorage(Storage* storage) {
	storages.push_back(storage);
}

void Room::ClearStorages() {
	for (auto &storage : storages) {
		if(storage)delete storage;
		storage = nullptr;
	}
	storages.clear();
}

vector<Manufacture*> Room::GetManufactures() const {
	return manufactures;
}

void Room::AddManufacture(Manufacture* manufacture) {
	manufactures.push_back(manufacture);
}

void Room::ClearManufactures() {
	for (auto &manufacture : manufactures) {
		if(manufacture)delete manufacture;
		manufacture = nullptr;
	}
	manufactures.clear();
}

int EmptyRoom::count = 0;

EmptyRoom::EmptyRoom() : id(count++) {

}

EmptyRoom::~EmptyRoom() {

}

const char* EmptyRoom::GetId() {
	return "empty";
}

const char* EmptyRoom::GetType() const {
	return "empty";
}

const char* EmptyRoom::GetName() {
	name = "空房间" + to_string(id);
	return name.data();
}

void EmptyRoom::ConfigRoom() {

}
