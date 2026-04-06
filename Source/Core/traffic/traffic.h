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

	// 初始化交通
	void Init(Map* map);

	// 释放空间
	void Destroy();

	// 时钟周期
	void Tick(Player* player);

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 载具工厂
	static VehicleFactory* vehicleFactory;

	// 站点工厂
	static StationFactory* stationFactory;

	// 线路工厂
	static RouteFactory* routeFactory;

private:
	// 全部站点
	OBJECT_HOLDER std::vector<Station*> stations;

	// 全部技能
	OBJECT_HOLDER std::vector<Route*> routes;
};

