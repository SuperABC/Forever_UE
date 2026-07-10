#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../map/geometry.h"

#include <string>
#include <functional>


class RouteMod {
public:
	/*
	* 构造路线模组
	*/
	RouteMod();

	/*
	* 析构路线模组
	*/
	virtual ~RouteMod();

	/*
	* Override
	* 统一类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 获取实例类型
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 获取实例名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 布局路线
	* @interfaces: 按交通类型分组的站点接口集合
	* @sizeX, sizeY: 地图尺寸
	* @nodeStaticCount: 节点静态计数起点
	*/
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
	/*
	* 注册线路类型
	* @id: 类型标识
	* @creator, deleter: 创建与析构函数
	*/
	void RegisterRoute(const std::string& id,
		std::function<RouteMod* ()> creator, std::function<void(RouteMod*)> deleter);

	/*
	* 清空注册表
	*/
	void RemoveAll();

	/*
	* 按类型创建线路对象
	* @id: 类型标识
	*/
	RouteMod* CreateRoute(const std::string& id) const;

	/*
	* 检查类型是否已注册
	* @id: 类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 类型名称
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取所有已启用的线路类型
	*/
	std::vector<std::string> GetRoutes() const;

	/*
	* 析构线路对象
	* @routeMod: 待析构的线路对象
	*/
	void DestroyRoute(RouteMod* routeMod) const;

private:
	// 注册表（类型标识 → 创建/析构函数对）
	std::unordered_map<
		std::string,
		std::pair<std::function<RouteMod* ()>, std::function<void(RouteMod*)>>
	> registries;

	// 启用配置（类型标识 → 是否启用）
	std::unordered_map<std::string, bool> configs;
};
