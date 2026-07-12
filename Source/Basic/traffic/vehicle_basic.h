#pragma once

#include "traffic/vehicle_mod.h"


// 城市载具
class CityVehicle : public VehicleMod {
public:
	/*
	* 构造城市载具
	*/
	CityVehicle();

	/*
	* 析构城市载具
	*/
	virtual ~CityVehicle();

	/*
	* 城市载具静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 城市载具动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 城市载具实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 自定义载具标签
	* @return: 载具标签
	*/
	static std::string GetLabel();
	
	/*
	* 设置属性
	*/
	virtual void SetupVehicle() override;

private:
	// 全局实例计数
	static int count;

	// 实例唯一编号
	int id;

	// 载具名称
	std::string name;
};

// 飞机载具
class PlaneVehicle : public VehicleMod {
public:
	/*
	* 构造飞机载具
	*/
	PlaneVehicle();

	/*
	* 析构飞机载具
	*/
	virtual ~PlaneVehicle();

	/*
	* 飞机载具静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 飞机载具动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 飞机载具实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 自定义载具标签
	* @return: 载具标签
	*/
	static std::string GetLabel();
	
	/*
	* 设置属性
	*/
	virtual void SetupVehicle() override;

private:
	// 全局实例计数
	static int count;

	// 实例唯一编号
	int id;

	// 载具名称
	std::string name;
};
