#include "room_base.h"

#include <sstream>
#include <iomanip>


using namespace std;

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
	// 全成员默认构造

}

Room::~Room() {
	// storage/manufacture在industry中统一创建和析构（运行中动态析构room存在泄漏）

}

Building* Room::GetParentBuilding() const {
	// 获取所在建筑
	return parentBuilding;
}

Component* Room::GetParentComponent() const {
	// 获取所在组合
	return parentComponent;
}

void Room::SetParent(Building* building) {
	// 设置所在建筑
	parentBuilding = building;
}

void Room::SetParent(Component* component) {
	// 设置所在组合
	parentComponent = component;
}

int Room::GetOwner() const {
	// 获取私人房东ID
	return owner;
}

void Room::SetOwner(int owner) {
	// 设置私人房东ID
	this->owner = owner;
}

bool Room::GetStated() const {
	// 获取是否由政府拥有
	return stated;
}

void Room::SetStated(bool stated) {
	// 设置是否由政府拥有
	this->stated = stated;
}

int Room::GetLayer() const {
	// 获取所在楼层
	return layer;
}

void Room::SetLayer(int layer) {
	// 设置所在楼层
	this->layer = layer;
}

const Room::WallHole& Room::GetDoors() const {
	// 获取门框
	return doors;
}

void Room::SetDoors(const WallHole& doors) {
	// 设置门框
	this->doors = doors;
}

const Room::WallHole& Room::GetWindows() const {
	// 获取窗框
	return windows;
}

void Room::SetWindows(const WallHole& windows) {
	// 设置窗框
	this->windows = windows;
}

const string& Room::GetAddress() const {
	// 获取门牌号字符串
	return address;
}

void Room::SetAddress(int number) {
	// 设置四位门牌号
	ostringstream oss;
	oss << setw(4) << setfill('0') << number;
	address = oss.str();
}

const vector<int>& Room::GetTenants() const {
	// 获取住户
	return tenants;
}

void Room::AddTenant(int id) {
	// 添加住户
	tenants.push_back(id);
}

bool Room::RemoveTenant(int id) {
	// 移除住户并返回是否找到
	for (auto& tenant : tenants) {
		if (tenant == id) {
			tenant = tenants.back();
			tenants.pop_back();
			return true;
		}
	}
	return false;
}

const vector<int>& Room::GetWorkers() const {
	// 获取工人
	return workers;
}

void Room::AddWorker(int id) {
	// 添加住户
	workers.push_back(id);
}

bool Room::RemoveWorker(int id) {
	// 移除工人并返回是否找到
	for (auto& worker : workers) {
		if (worker == id) {
			worker = workers.back();
			workers.pop_back();
			return true;
		}
	}
	return false;
}

Storage* Room::GetStorage() const {
	// 获取仓库
	return storage;
}

void Room::SetStorage(Storage* storage) {
	// 设置仓库
	if (storage == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Storage is null when attaching to room.");
	}
	this->storage = storage;
}

Storage* Room::RemoveStorage() {
	// 移除仓库并返回
	auto item = storage;
	storage = nullptr;
	return item;
}

const vector<Manufacture*>& Room::GetManufactures() const {
	// 获取工坊
	return manufactures;
}

void Room::AddManufacture(Manufacture* manufacture) {
	// 添加工坊
	if (manufacture == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Manufacture is null when attaching to room.");
	}
	manufactures.push_back(manufacture);
}

Manufacture* Room::RemoveManufacture(const string& name) {
	// 移除第一个符合名字的工坊并返回
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

vector<Manufacture*> Room::ClearManufactures() {
	// 清空工坊并返回
	auto items = manufactures;
	manufactures.clear();
	return items;
}

void RoomFactory::RegisterRoom(const string& id,
	function<Room* ()> creator, function<void(Room*)> deleter) {
	// 注册构造器和析构器
	registries[id] = { creator, deleter };
}

Room* RoomFactory::CreateRoom(const string& id) {
	// 根据配置构造房间
	auto configIt = configs.find(id);
	if (configIt == configs.end() || !configIt->second) {
		debugf("Room %s not enabled or not configured.\n", id.c_str());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it != registries.end()) {
		Room* room = it->second.first();
		if (room == nullptr) {
			debugf("Creat room %s failed.\n", id.c_str());
		}
		return room;
	}

	debugf("Room %s not registered.\n", id.c_str());
	return nullptr;
}

bool RoomFactory::CheckRegistered(const string& id) {
	// 检查是否注册
	return registries.find(id) != registries.end();
}

void RoomFactory::SetConfig(const string& name, bool config) {
	// 设置配置
	configs[name] = config;
}

void RoomFactory::DestroyRoom(Room* room) const {
	// 析构房间
	if (!room) {
		return;
	}
	auto it = registries.find(room->GetType());
	if (it != registries.end()) {
		it->second.second(room);
	}
	else {
		debugf(("Deleter not found for " + room->GetType() + ".\n").data());
	}
}