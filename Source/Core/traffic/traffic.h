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

	// 读取 Mods
	void InitVehicles(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitStations(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
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

	// 统一工厂
	static VehicleFactory* vehicleFactory;
	static StationFactory* stationFactory;
	static RouteFactory* routeFactory;

private:
	// 全部站点
	OBJECT_HOLDER std::vector<Station*> stations;

	// 全部技能
	OBJECT_HOLDER std::vector<Route*> routes;
};

