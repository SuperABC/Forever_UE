#include "room.h"

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
	number("") {

}

Room::~Room() {
	factory->DestroyRoom(mod);
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

// 设置所在建筑
void Room::SetParent(Building* building) {
	parentBuilding = building;
}

// 获取所在组合
Component* Room::GetParentComponent() const {
	return parentComponent;
}

// 设置所在组合
void Room::SetParent(Component* component) {
	parentComponent = component;
}

// 获取所在楼层
int Room::GetLayer() const {
	return layer;
}

// 设置所在楼层
void Room::SetLayer(int layer) {
	this->layer = layer;
}

// 获取门框
const Room::WallHole& Room::GetDoors() const {
	return doors;
}

// 设置门框
void Room::SetDoors(const WallHole& doors) {
	this->doors = doors;
}

// 获取窗框
const Room::WallHole& Room::GetWindows() const {
	return windows;
}

// 设置窗框
void Room::SetWindows(const WallHole& windows) {
	this->windows = windows;
}

// 获取门牌号
string Room::GetNumber() const {
	return number;
}

// 设置四位门牌号
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

// 获取完整地址
string Room::GetAddress() const {
	return GetParentBuilding()->GetAddress() + " " + number;
}

// 获取世界坐标
std::pair<float, float> Room::GetPosition(float x, float y) const {
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

pair<string, float> Room::StorageConfig() const {
	return mod->storageConfig;
}

vector<string> Room::ManufactureTypes() const {
	return mod->manufactureTypes;
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
