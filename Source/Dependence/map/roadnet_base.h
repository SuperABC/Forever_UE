#pragma once

#include "../common/plot.h"
#include "../traffic/route_base.h"
#include "../traffic/station_base.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>


class Roadnet {
public:
	Roadnet();
	virtual ~Roadnet();

	// 子类实现方法

	// 统一类型定义
	static std::string GetId();
	virtual std::string GetType() const = 0;
	virtual std::string GetName() const = 0;

	// 在平原上生成路网
	virtual void DistributeRoadnet(int width, int height,
		std::function<std::string(int, int)> get, StationFactory* stationFactory, RouteFactory* routeFactory) = 0;

	// 父类实现方法

	// 获取所有节点
	const std::vector<Node>& GetNodes() const;

	// 获取所有公路
	const std::vector<Connection>& GetConnections() const;

	// 获取所有地块
	const std::vector<Plot*>& GetPlots() const;

	// 分配地块地址
	void AllocateAddress();

	// 寻址地块
	Plot* LocatePlot(const std::string& road, int id) const;

	// 自动寻路
	const std::vector<Connection> AutoNavigate(
		const std::vector<std::pair<Connection, float>>& startRoads, const std::vector<std::pair<Connection, float>>& endRoads) const;
	const std::vector<Connection> AutoNavigate(
		const Plot* startPlot, const Plot* endPlot) const;
	const std::vector<Connection> AutoNavigate(
		const std::vector<std::pair<Connection, float>>& startRoads, const Plot* endPlot) const;
	const std::vector<Connection> AutoNavigate(
		const Plot* startPlot, const std::vector<std::pair<Connection, float>>& endRoads) const;

protected:
	// 节点
	std::vector<Node> nodes;

	// 公路
	std::vector<Connection> connections;

	// 地块
	std::vector<Plot*> plots;

	// 非公路连接
	std::vector<Route*> routes;

private:
	// 地块地址簿
	std::unordered_map<std::string, std::vector<Plot*>> addresses;
};

class RoadnetFactory {
public:
	// 注册路网
	void RegisterRoadnet(const std::string& id,
		std::function<Roadnet* ()> creator, std::function<void(Roadnet*)> deleter);

	// 创建路网（包含new操作）
	Roadnet* CreateRoadnet(const std::string& id);

	// 检查是否注册
	bool CheckRegistered(const std::string& id);

	// 设置启用配置
	void SetConfig(std::string name, bool config);

	// 获取启用路网
	Roadnet* GetRoadnet() const;

	// 析构路网（包含delete操作）
	void DestroyRoadnet(Roadnet* roadnet) const;

private:
	std::unordered_map<
		std::string,
		std::pair<std::function<Roadnet* ()>, std::function<void(Roadnet*)>>
	> registries;
	std::unordered_map<std::string, bool> configs;
};