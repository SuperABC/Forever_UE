#pragma once

#include "room_base.h"
#include "building_base.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>


class Building;
class Room;

class Component {
public:
	Component();
	virtual ~Component();

	// 子类实现方法

	// 统一类型定义
	static std::string GetId();
	virtual std::string GetType() const = 0;
	virtual std::string GetName() const = 0;

	// 父类实现方法

	// 获取所在建筑
	Building* GetParent() const;

	// 设置所在建筑
	void SetParent(Building* building);

	// 获取所有包含房间
	std::vector<Room*>& GetRooms();

	// 添加房间
	void AddRoom(Room* room);

private:
	// 所在建筑
	Building* parentBuilding;

	// 包含房间
	std::vector<Room*> rooms;
};

class ComponentFactory {
public:
	// 构造组合（包含new操作）
	void RegisterComponent(const std::string& id,
		std::function<Component* ()> creator, std::function<void(Component*)> deleter);

	// 创建组合
	Component* CreateComponent(const std::string& id);

	// 检查是否注册
	bool CheckRegistered(const std::string& id);

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构组合（包含delete操作）
	void DestroyComponent(Component* component) const;

private:
	std::unordered_map<
		std::string,
		std::pair<std::function<Component* ()>, std::function<void(Component*)>>
	> registries;
	std::unordered_map<std::string, bool> configs;
};