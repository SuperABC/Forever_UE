#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Traffic {
public:
	/*
	* 构造交通系统
	*/
	Traffic();

	/*
	* 析构交通系统
	*/
	~Traffic();

	/*
	* 读取配置文件
	*/
	void LoadConfigs() const;

	/*
	* 读取并注册载具模组
	* @modHandles: 模组句柄表
	* @dlls: 模组路径列表
	*/
	void InitVehicles(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取并注册站点模组
	* @modHandles: 模组句柄表
	* @dlls: 模组路径列表
	*/
	void InitStations(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取并注册线路模组
	* @modHandles: 模组句柄表
	* @dlls: 模组路径列表
	*/
	void InitRoutes(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 初始化站点建筑
	* @map: 地图对象
	*/
	void InitBuildings(Map* map);

	/*
	* 初始化车辆与路线
	* @map: 地图对象
	* @populace: 人口对象
	*/
	void InitTraffic(Map* map, Populace* populace);

	/*
	* 释放全部交通对象
	*/
	void Destroy();

	/*
	* 时钟周期更新
	* @player: 玩家对象
	*/
	void Tick(Player* player);

	/*
	* 应用剧情变化
	* @change: 变化事件
	* @getValues: 脚本变量获取函数集合
	*/
	void ApplyChange(Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	/*
	* 获取全部车辆
	*/
	std::vector<Vehicle*> GetVehicles() const;

	/*
	* 按名称获取车辆
	* @name: 车辆名称
	*/
	Vehicle* GetVehicle(const std::string& name) const;

	/*
	* 添加车辆
	* @name: 车辆名称
	* @vehicle: 车辆对象
	*/
	void AddVehicle(std::string name, Vehicle* vehicle);

	/*
	* 移除车辆
	* @name: 车辆名称
	*/
	void RemoveVehicle(std::string name);

	// 载具工厂
	static VehicleFactory* vehicleFactory;

	// 站点工厂
	static StationFactory* stationFactory;

	// 线路工厂
	static RouteFactory* routeFactory;

private:
	// 全部路线
	OBJECT_HOLDER std::unordered_map<std::string, Route*> routes;

	// 全部站点
	OBJECT_HOLDER std::unordered_map<std::string, Station*> stations;

	// 全部载具
	OBJECT_HOLDER std::unordered_map<std::string, Vehicle*> vehicles;
};
