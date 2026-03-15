#pragma once

#include "../map/map.h"
#include "../story/story.h"

#include "route.h"
#include "station.h"
#include "vehicle.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Traffic {
public:
	Traffic();
	~Traffic();

	// 设置资源路径
	void SetResourcePath(const std::string& path);

	// 读取Mods
	void InitRoutes(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitStations(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitVehicles(std::unordered_map<std::string, HMODULE>& modHandles);

	// 初始化全部交通
	void Init(Map* map);

	// 读取配置文件
	void ReadConfigs(const std::string& path) const;

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前交通
	void Print() const;

	// 保存/加载交通
	void Load(const std::string& path);
	void Save(const std::string& path) const;

	// 应用变更
	void ApplyChange(Change* change, Story* story,
		std::vector<std::function<std::pair<bool, ValueType>(
			const std::string&)>>&getValues);

	// 获取工厂
	RouteFactory* GetRouteFactory() const;
	StationFactory* GetStationFactory() const;

private:
	// 资源路径
	std::string resourcePath;

	// Mod管理
	static RouteFactory* routeFactory;
	static StationFactory* stationFactory;
	static VehicleFactory* vehicleFactory;
};

