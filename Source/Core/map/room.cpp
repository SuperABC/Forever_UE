#include "room.h"

#include "map/map.h"
#include "map/block.h"
#include "map/zone.h"
#include "map/building.h"
#include "map/component.h"
#include "populace/person.h"
#include "industry/storage.h"
#include "industry/manufacture.h"
#include "player/asset.h"

#include <sstream>
#include <iomanip>


using namespace std;

Room::Room(RoomFactory* factory, const string& room) :
	mod(factory->CreateRoom(room)),
	factory(factory),
	type(),
	name(),
	parentBuilding(nullptr),
	parentComponent(nullptr),
	layer(0),
	direction(FACE_WEST),
	doors(),
	windows(),
	number(""),
	pivots(),
	navigationNode(nullptr),
	stated(false),
	owner(nullptr),
	tenants(),
	workers(),
	storages(),
	manufactures(),
	parkings() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Room " + room + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Room::~Room() {
	factory->DestroyRoom(mod);

	for (auto& pivot : pivots) {
		delete pivot;
	}
	pivots.clear();

	delete navigationNode;
	navigationNode = nullptr;

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

FACE_DIRECTION Room::GetDirection() const {
	return direction;
}

void Room::SetDirection(FACE_DIRECTION direction) {
	this->direction = direction;
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
	mod->ConfigRoom(this, direction);

	for(auto parking : mod->parkingSpaces) {
		if(parking.size() == 8) {
			Quad quad;
			quad.SetVertices(
				parking[0] * GetSizeX() + parking[1], parking[2] * GetSizeY() + parking[3],
				parking[4] * GetSizeX() + parking[5], parking[6] * GetSizeY() + parking[7]);
			parkings.push_back(quad);
		}
	}
}

string Room::GetAddress() const {
	return GetParentBuilding()->GetAddress() + " " + number;
}

const vector<Node*> Room::GetPivots() const {
	return pivots;
}

void Room::PlacePivots(Quad* room) {
	mod->PlacePivots(room);

	for (auto& pivot : mod->pivots) {
		float x = pivot[0] * room->GetSizeX() + pivot[1];
		float y = pivot[2] * room->GetSizeY() + pivot[3];
		auto [wx, wy] = GetPosition(x, y);
		pivots.push_back(new Node("room", wx, wy));
	}
}

Node* Room::GetNavigationNode() const {
	return navigationNode;
}

void Room::SetNavigationNode(Node* node) {
	delete navigationNode;
	navigationNode = node;
}

pair<float, float> Room::GetPosition() const {
	return GetPosition(GetSizeX() / 2.f, GetSizeY() / 2.f);
}

pair<float, float> Room::GetPosition(float x, float y) const {
	auto building = GetParentBuilding();
	if (!building) {
		THROW_EXCEPTION(NullPointerException, "Room parent building is null.\n");
	}
	auto block = building->GetParentBlock();
	if (!block) {
		THROW_EXCEPTION(NullPointerException, "Room parent block is null.\n");
	}
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

bool Room::IsParking() const {
	return mod->isParking;
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

vector<vector<float>> Room::ParkingSpaces() const {
	return mod->parkingSpaces;
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

vector<Quad> Room::GetParkings() const {
	return parkings;
}

vector<Vehicle*> Room::GetVehicles() const {
	return vehicles;
}

void Room::AddVehicle(Vehicle* vehicle) {
	vehicles.push_back(vehicle);
}

void Room::RemoveVehicle(Vehicle* vehicle) {
	for (auto it = vehicles.begin(); it != vehicles.end(); ++it) {
		if (*it == vehicle) {
			vehicles.erase(it);
			return;
		}
	}
}

void Room::ClearVehicles() {
	vehicles.clear();
}

const vector<Asset*>& Room::GetAssets() const {
	return assets;
}

void Room::AddAsset(Asset* asset) {
	assets.push_back(asset);
}

Asset* Room::RemoveAsset(const string& name) {
	for (auto it = assets.begin(); it != assets.end(); ++it) {
		if ((*it)->GetName() == name) {
			Asset* removed = *it;
			assets.erase(it);
			return removed;
		}
	}
	return nullptr;
}

void Room::ClearAssets() {
	for (auto asset : assets) delete asset;
	assets.clear();
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

void EmptyRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {

}

void EmptyRoom::PlacePivots(Quad* room) {

}
