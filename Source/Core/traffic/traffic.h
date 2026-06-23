#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Traffic {
public:
	// 构造交通
	Traffic();

	// 析构交通
	~Traffic();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取载具模组
	void InitVehicles(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取站点模组
	void InitStations(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取线路模组
	void InitRoutes(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化公共交通
	void InitPublic(Map* map);

	// 初始化车辆
	void InitPrivate(Map* map, Populace* populace);

	// 释放空间
	void Destroy();

	// 时钟周期
	void Tick(Player* player);

	// 应用剧情变化
	void ApplyChange(Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	// 获取全部车辆
	std::vector<Vehicle*> GetVehicles() const;

	// 按名称获取车辆
	Vehicle* GetVehicle(const std::string& name) const;

	// 添加车辆
	void AddVehicle(std::string name, Vehicle* vehicle);

	// 移除车辆
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

	// 全部载具
	OBJECT_HOLDER std::unordered_map<std::string, Vehicle*> vehicles;
};

