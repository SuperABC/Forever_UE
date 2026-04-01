#pragma once

#include "component_mod.h"

#include "building.h"


// 子类注册函数
typedef void (*RegisterModComponentsFunc)(ComponentFactory* factory);

// 组合实体
class ComponentMod;
class Building;
class Room;
class ComponentFactory;
class Component {
public:
	// 禁止默认构造
	Component() = delete;

	// 通过类型从工厂构造
	Component(ComponentFactory* factory, const std::string& component);

	// 析构组合
	~Component();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取所在建筑
	Building* GetParent() const;

	// 设置所在建筑
	void SetParent(Building* building);

	// 获取所有包含房间
	std::vector<Room*>& GetRooms();

	// 添加房间
	void AddRoom(Room* room);

private:
	// 模组对象
	OBJECT_HOLDER ComponentMod* mod;

	// 工厂
	ComponentFactory* factory;

	// 组合类型
	std::string type;

	// 组合名称
	std::string name;

	// 所在建筑
	Building* parentBuilding;

	// 包含房间
	std::vector<Room*> rooms;
};

// 空组合
class EmptyComponent : public ComponentMod {
public:
	EmptyComponent();
	virtual ~EmptyComponent();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

