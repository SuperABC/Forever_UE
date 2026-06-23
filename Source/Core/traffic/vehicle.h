#pragma once

#include "traffic/vehicle_mod.h"

#include "class.h"

#include <unordered_set>


// 子类注册函数
typedef void (*RegisterModVehiclesFunc)(VehicleFactory* factory);

// 载具实体
class Vehicle {
public:
	// 禁止默认构造
	Vehicle() = delete;

	// 通过标签从工厂构造
	Vehicle(VehicleFactory* factory, const std::string& label);

	// 析构载具
	~Vehicle();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 获取资产路径
	std::string GetPath() const;

	// 获取所在房间
	Room* GetRoom() const;

	// 获取位置
	std::vector<float> GetPosition() const;

	// 进入房间
	void EnterRoom(Room* room);

	// 离开房间
	void LeaveRoom();

	// 设置位置
	void SetPosition(float x, float y, float z, float r);

	// 定义载具
	void SetupVehicle();

	// 获取剧本
	Script* GetScript() const;

	// 添加选项
	bool AddOption(const std::string& option);

	// 移除选项
	bool RemoveOption(const std::string& option);

	// 获取选项
	std::unordered_set<std::string> GetOptions() const;

private:
	// 模组对象
	OBJECT_HOLDER VehicleMod* mod;

	// 工厂
	VehicleFactory* factory;

	// 载具类型
	std::string type;

	// 载具名称
	std::string name;

	// 载具资产路径
	std::string path;

	// 载具宽度
	float width;

	// 载具长度
	float length;

	// 拥有者
	Person* owner;

	// 司机
	Person* driver;

	// 当前所在房间
	Room* room;

	// 当前位置
	std::vector<float> xyzr;

	// 关联剧情
	OBJECT_HOLDER Script* script;

	// 交互选项
	std::unordered_set<std::string> options;
};

// 空载具
class EmptyVehicle : public VehicleMod {
public:
	EmptyVehicle();
	virtual ~EmptyVehicle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static std::string GetLabel();

	virtual void SetupVehicle()override;

private:
	static int count;

	int id;
	std::string name;
};

