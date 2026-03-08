#include "component_base.h"
#include "building_base.h"


using namespace std;

Component::Component() :
	parentBuilding(nullptr) {
	// 全成员默认构造

}

Component::~Component() {
	// room/building在map中统一创建和析构

}

Building* Component::GetParent() const {
	// 获取所在建筑
	return parentBuilding;
}

void Component::SetParent(Building* building) {
	// 设置所在建筑
	parentBuilding = building;
}

vector<Room*>& Component::GetRooms() {
	// 获取所有包含房间
	return rooms;
}

void Component::AddRoom(Room* room) {
	// 添加房间
	if (room == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Room is null when adding to component.");
	}
	rooms.push_back(room);
}

void ComponentFactory::RegisterComponent(const string& id,
	function<Component* ()> creator, function<void(Component*)> deleter) {
	// 注册构造器和析构器
	registries[id] = { creator, deleter };
}

Component* ComponentFactory::CreateComponent(const string& id) {
	// 根据配置构造组合
	auto configIt = configs.find(id);
	if (configIt == configs.end() || !configIt->second) {
		debugf("Component %s not enabled or not configured.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it != registries.end()) {
		Component* component = it->second.first();
		if (component == nullptr) {
			debugf("Creat component %s failed.\n", id.data());
		}
		return component;
	}

	debugf("Component %s not registered.\n", id.data());
	return nullptr;
}

bool ComponentFactory::CheckRegistered(const string& id) {
	// 检查是否注册
	return registries.find(id) != registries.end();
}

void ComponentFactory::SetConfig(const string& name, bool config) {
	// 设置启用配置
	configs[name] = config;
}

void ComponentFactory::DestroyComponent(Component* component) const {
	// 析构组合
	if (component == nullptr) {
		return;
	}
	auto it = registries.find(component->GetType());
	if (it != registries.end()) {
		it->second.second(component);
	}
	else {
		debugf("Deleter not found for %s.\n", component->GetType().data());
	}
}