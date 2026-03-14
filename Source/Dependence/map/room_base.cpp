#include "../common/error.h"
#include "../common/utility.h"

#include "room_base.h"

#include <sstream>
#include <iomanip>


using namespace std;

// 全成员默认构造
Room::Room() :
	parentBuilding(nullptr),
	parentComponent(nullptr),
	stated(false),
	owner(-1),
	layer(0),
	doors(),
	windows(),
	address(),
	tenants(),
	workers(),
	storage(nullptr),
	manufactures() {

}

// storage/manufacture在industry中统一创建和析构
Room::~Room() {

}

// 获取所在建筑
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
int Room::GetOwner() const {
	return owner;
}

// 设置私人房东ID
void Room::SetOwner(int owner) {
	this->owner = owner;
}

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
	return address;
}

// 设置四位门牌号
void Room::SetNumber(int floor, int number) {
	ostringstream oss;
	if(floor < 0){
		oss << 'b' << -floor;
	}
	else{
		oss << floor;
	}
	oss << setw(4) << setfill('0') << number;
	address = oss.str();
}

// 获取完整地址
string Room::GetAddress() const {
	return GetParentBuilding()->GetAddress() + " " + address;
}

// 获取住户
const vector<int>& Room::GetTenants() const {
	return tenants;
}

// 添加住户
void Room::AddTenant(int id) {
	tenants.push_back(id);
}

// 移除住户并返回是否找到
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

// 获取工人
const vector<int>& Room::GetWorkers() const {
	return workers;
}

// 添加住户
void Room::AddWorker(int id) {
	workers.push_back(id);
}

// 移除工人并返回是否找到
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

// 获取仓库
Storage* Room::GetStorage() const {
	return storage;
}

// 设置仓库
void Room::SetStorage(Storage* storage) {
	if (storage == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Storage is null when attaching to room.");
	}
	this->storage = storage;
}

// 移除仓库并返回
Storage* Room::RemoveStorage() {
	auto item = storage;
	storage = nullptr;
	return item;
}

// 获取工坊
const vector<Manufacture*>& Room::GetManufactures() const {
	return manufactures;
}

// 添加工坊
void Room::AddManufacture(Manufacture* manufacture) {
	if (manufacture == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Manufacture is null when attaching to room.");
	}
	manufactures.push_back(manufacture);
}

// 移除第一个符合名字的工坊并返回
Manufacture* Room::RemoveManufacture(const string& name) {
	for (auto& manufacture : manufactures) {
		if (manufacture->GetName() == name) {
			auto item = manufacture;
			manufacture = manufactures.back();
			manufactures.pop_back();
			return item;
		}
	}
	return nullptr;
}

// 清空工坊并返回
vector<Manufacture*> Room::ClearManufactures() {
	auto items = manufactures;
	manufactures.clear();
	return items;
}

// 获取世界坐标
std::pair<float, float> Room::GetPosition(float x, float y) const {
	auto building = GetParentBuilding();
	auto plot = GetParentBuilding()->GetParentPlot();
	auto zone = building->GetParentZone();
	if (zone) {
		float plotX = zone->GetPosX() - zone->GetSizeX() / 2.f +
			building->GetPosX() - building->GetSizeX() / 2.f +
			building->GetConstruction().GetPosX() - building->GetConstruction().GetSizeX() / 2.f +
			GetPosX() - GetSizeX() / 2.f + x;
		float plotY = zone->GetPosY() - zone->GetSizeY() / 2.f +
			building->GetPosY() - building->GetSizeY() / 2.f +
			building->GetConstruction().GetPosY() - building->GetConstruction().GetSizeY() / 2.f +
			GetPosY() - GetSizeY() / 2.f + y;
		return plot->GetPosition(plotX, plotY);
	}
	else {
		float plotX = building->GetPosX() - building->GetSizeX() / 2.f +
			building->GetConstruction().GetPosX() - building->GetConstruction().GetSizeX() / 2.f +
			GetPosX() - GetSizeX() / 2.f + x;
		float plotY = building->GetPosY() - building->GetSizeY() / 2.f +
			building->GetConstruction().GetPosY() - building->GetConstruction().GetSizeY() / 2.f +
			GetPosY() - GetSizeY() / 2.f + y;
		return plot->GetPosition(plotX, plotY);
	}
	return { 0.f, 0.f };
}

// 注册房间
void RoomFactory::RegisterRoom(const string& id,
    function<Room* ()> creator, function<void(Room*)> deleter) {
    registries[id] = { creator, deleter };
}

// 创建房间
Room* RoomFactory::CreateRoom(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Room %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Room %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Room " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool RoomFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void RoomFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构房间
void RoomFactory::DestroyRoom(Room* room) const {
    if (!room) {
        debugf("Warning: Room is null when deleting.\n");
        return;
    }

    auto it = registries.find(room->GetType());
    if (it == registries.end()) {
        debugf("Warning: Room %s not registered when deleting.\n", room->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(room);
    } else {
        THROW_EXCEPTION(NullPointerException, "Room " + room->GetType() + " deleter is null.\n");
    }
}

