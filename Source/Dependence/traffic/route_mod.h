#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../map/geometry.h"

#include <string>
#include <functional>


class RouteMod {
public:
	// 无构造
	RouteMod();

	// 无析构
	virtual ~RouteMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 布局路线
	COSTOM_INIT virtual void LayoutRoute(
		const std::unordered_map<std::string, std::vector<std::pair<std::vector<Node*>, std::vector<std::pair<Node*, Node*>>>>>& interfaces,
		int sizeX, int sizeY, int nodeStaticCount) = 0;

	// 外部连接
	std::vector<Node> externs;

	// 路线节点
	std::vector<Node> nodes;

	// 路线连接
	std::vector<Connection> connections;
};

class RouteFactory {
public:
	// 注册线路
	void RegisterRoute(const std::string& id,
		std::function<RouteMod* ()> creator, std::function<void(RouteMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建线路
	RouteMod* CreateRoute(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取所有启用线路
	std::vector<std::string> GetRoutes() const;

	// 析构线路
	void DestroyRoute(RouteMod* routeMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<RouteMod* ()>, std::function<void(RouteMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

