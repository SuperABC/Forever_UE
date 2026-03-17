#include "../common/error.h"
#include "../common/utility.h"

#include "component_base.h"


using namespace std;

// 全成员默认构造
Component::Component() :
	parentBuilding(nullptr) {

}

// room/building在map中统一创建和析构
Component::~Component() {

}

// 获取所在建筑
Building* Component::GetParent() const {
	return parentBuilding;
}

// 设置所在建筑
void Component::SetParent(Building* building) {
	parentBuilding = building;
}

// 获取所有包含房间
vector<Room*>& Component::GetRooms() {
	return rooms;
}

// 添加房间
void Component::AddRoom(Room* room) {
	if (room == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Room is null when adding to component.");
	}
	rooms.push_back(room);
}

// 清空注册
void ComponentFactory::RemoveAll() {
    for(auto &config : configs) {
        config.second = false;
    }
}

// 注册组合
void ComponentFactory::RegisterComponent(const string& id,
    function<Component* ()> creator, function<void(Component*)> deleter) {
    registries[id] = { creator, deleter };
}

// 创建组合
Component* ComponentFactory::CreateComponent(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Component %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Component %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Component " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool ComponentFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void ComponentFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构组合
void ComponentFactory::DestroyComponent(Component* component) const {
    if (!component) {
        debugf("Warning: Component is null when deleting.\n");
        return;
    }

    auto it = registries.find(component->GetType());
    if (it == registries.end()) {
        debugf("Warning: Component %s not registered when deleting.\n", component->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(component);
    } else {
        THROW_EXCEPTION(NullPointerException, "Component " + component->GetType() + " deleter is null.\n");
    }
}

