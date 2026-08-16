#pragma once

#include "map/component_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModComponentsFunc)(ComponentFactory* factory);

// 子类注册收尾函数
typedef void (*FinishModComponentsFunc)(ComponentFactory* factory);

// 组合实体
class Component {
public:
	/*
	* 禁止默认构造
	*/
	Component() = delete;

	/*
	* 通过组合静态类型标识从工厂构造组合
	* @factory: 组合工厂
	* @component: 组合静态类型标识
	*/
	Component(ComponentFactory* factory, const std::string& component);

	/*
	* 析构组合
	*/
	~Component();

	/*
	* 获取组合类型
	*/
	std::string GetType() const;

	/*
	* 获取组合名称
	*/
	std::string GetName() const;

	/*
	* 获取所在建筑
	*/
	Building* GetParent() const;

	/*
	* 设置所在建筑
	* @building: 所在建筑
	*/
	void SetParent(Building* building);

	/*
	* 获取所有包含房间
	*/
	std::vector<Room*>& GetRooms();

	/*
	* 添加房间
	* @room: 房间
	*/
	void AddRoom(Room* room);

	/*
	* 获取关联剧情
	*/
	Script* GetScript() const;

	/*
	* 初始化组合
	*/
	void InitComponent();

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

	// 关联剧情
	OBJECT_HOLDER Script* script;
};

// 空组合
class EmptyComponent : public ComponentMod {
public:
	/*
	* 构造空组合
	*/
	EmptyComponent();

	/*
	* 析构空组合
	*/
	virtual ~EmptyComponent();

	/*
	* Override
	* 组合静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 组合动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 组合实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 初始化组合
	*/
	virtual void InitComponent() override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

