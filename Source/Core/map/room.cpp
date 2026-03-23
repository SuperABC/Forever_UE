#include "room.h"

#include <sstream>
#include <iomanip>


using namespace std;

Room::Room(RoomFactory* factory, string room) :
	mod(factory->CreateRoom(room)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	parentBuilding(nullptr),
	parentComponent(nullptr),
	layer(0),
	stated(false),
	//owner(nullptr),
	doors(),
	windows(),
	number("")//,
	//tenants(),
	//workers(),
	//storage(),
	{//manufactures() {

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

Quad* Room::GetQuad() {
	return mod;
}

Building* Room::GetParentBuilding() const {
	return parentBuilding;
}

// 获取所在组合
Component* Room::GetParentComponent() const {
	return parentComponent;
}

// 设置所在建筑
void Room::SetParent(Building* building) {
	parentBuilding = building;
}

// 设置所在组合
void Room::SetParent(Component* component) {
	parentComponent = component;
}

// 获取私人房东ID
//int Room::GetOwner() const {
//	return owner;
//}

// 设置私人房东ID
//void Room::SetOwner(int owner) {
//	this->owner = owner;
//}

// 获取是否由政府拥有
bool Room::GetStated() const {
	return stated;
}

// 设置是否由政府拥有
void Room::SetStated(bool stated) {
	this->stated = stated;
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

// 获取住户
//const vector<int>& Room::GetTenants() const {
//	return tenants;
//}

// 添加住户
//void Room::AddTenant(int id) {
//	tenants.push_back(id);
//}

// 移除住户并返回是否找到
//bool Room::RemoveTenant(int id) {
//	for (auto& tenant : tenants) {
//		if (tenant == id) {
//			tenant = tenants.back();
//			tenants.pop_back();
//			return true;
//		}
//	}
//	return false;
//}

// 获取工人
//const vector<int>& Room::GetWorkers() const {
//	return workers;
//}

// 添加住户
//void Room::AddWorker(int id) {
//	workers.push_back(id);
//}

// 移除工人并返回是否找到
//bool Room::RemoveWorker(int id) {
//	for (auto& worker : workers) {
//		if (worker == id) {
//			worker = workers.back();
//			workers.pop_back();
//			return true;
//		}
//	}
//	return false;
//}

// 获取仓库
//Storage* Room::GetStorage() const {
//	return storage;
//}

// 设置仓库
//void Room::SetStorage(Storage* storage) {
//	if (storage == nullptr) {
//		THROW_EXCEPTION(NullPointerException, "Storage is null when attaching to room.");
//	}
//	this->storage = storage;
//}

// 移除仓库并返回
//Storage* Room::RemoveStorage() {
//	auto item = storage;
//	storage = nullptr;
//	return item;
//}

// 获取工坊
//const vector<Manufacture*>& Room::GetManufactures() const {
//	return manufactures;
//}

// 添加工坊
//void Room::AddManufacture(Manufacture* manufacture) {
//	if (manufacture == nullptr) {
//		THROW_EXCEPTION(NullPointerException, "Manufacture is null when attaching to room.");
//	}
//	manufactures.push_back(manufacture);
//}

// 移除第一个符合名字的工坊并返回
//Manufacture* Room::RemoveManufacture(const string& name) {
//	for (auto& manufacture : manufactures) {
//		if (manufacture->GetName() == name) {
//			auto item = manufacture;
//			manufacture = manufactures.back();
//			manufactures.pop_back();
//			return item;
//		}
//	}
//	return nullptr;
//}

// 清空工坊并返回
//vector<Manufacture*> Room::ClearManufactures() {
//	auto items = manufactures;
//	manufactures.clear();
//	return items;
//}

// 获取世界坐标
std::pair<float, float> Room::GetPosition(float x, float y) const {
	auto building = GetParentBuilding();
	auto block = GetParentBuilding()->GetParentBlock();
	auto zone = building->GetParentZone();
	if (zone) {
		float blockX = zone->GetQuad()->GetPosX() - zone->GetQuad()->GetSizeX() / 2.f +
			building->GetQuad()->GetPosX() - building->GetQuad()->GetSizeX() / 2.f +
			building->GetConstruction().GetPosX() - building->GetConstruction().GetSizeX() / 2.f +
			mod->GetPosX() - mod->GetSizeX() / 2.f + x;
		float blockY = zone->GetQuad()->GetPosY() - zone->GetQuad()->GetSizeY() / 2.f +
			building->GetQuad()->GetPosY() - building->GetQuad()->GetSizeY() / 2.f +
			building->GetConstruction().GetPosY() - building->GetConstruction().GetSizeY() / 2.f +
			mod->GetPosY() - mod->GetSizeY() / 2.f + y;
		return block->GetPosition(blockX, blockY);
	}
	else {
		float blockX = building->GetQuad()->GetPosX() - building->GetQuad()->GetSizeX() / 2.f +
			building->GetConstruction().GetPosX() - building->GetConstruction().GetSizeX() / 2.f +
			mod->GetPosX() - mod->GetSizeX() / 2.f + x;
		float blockY = building->GetQuad()->GetPosY() - building->GetQuad()->GetSizeY() / 2.f +
			building->GetConstruction().GetPosY() - building->GetConstruction().GetSizeY() / 2.f +
			mod->GetPosY() - mod->GetSizeY() / 2.f + y;
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
