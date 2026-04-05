#pragma once

#include "vehicle_mod.h"


// 子类注册函数
typedef void (*RegisterModVehiclesFunc)(VehicleFactory* factory);

// 载具实体
class Vehicle {
public:
	// 禁止默认构造
	Vehicle() = delete;

	// 通过类型从工厂构造
	Vehicle(VehicleFactory* factory, const std::string& vehicle);

	// 析构载具
	~Vehicle();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

private:
	// 模组对象
	OBJECT_HOLDER VehicleMod* mod;

	// 工厂
	VehicleFactory* factory;

	// 载具类型
	std::string type;

	// 载具名称
	std::string name;

};

// 空载具
class EmptyVehicle : public VehicleMod {
public:
	EmptyVehicle();
	virtual ~EmptyVehicle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

