#pragma once

#include "traffic/route_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModRoutesFunc)(RouteFactory* factory);

// 线路实体
class Route {
public:
	/*
	* 禁止默认构造
	*/
	Route() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 线路工厂
	* @route: 线路类型标识
	*/
	Route(RouteFactory* factory, const std::string& route);

	/*
	* 析构线路
	*/
	~Route();

	/*
	* 线路动态类型标识
	* @return: 动态类型标识
	*/
	std::string GetType() const;

	/*
	* 线路实例唯一名称
	* @return: 实例唯一名称
	*/
	std::string GetName() const;

	/*
	* 布局路线
	* @interfaces: 按交通类型分组的站点接口集合
	* @sizeX, sizeY: 地图尺寸
	* @nodeStaticCount: 节点静态计数起点
	*/
	void LayoutRoute(
		const std::unordered_map<std::string, std::vector<std::pair<std::vector<Node*>, std::vector<std::pair<Node*, Node*>>>>>& interfaces,
		int sizeX, int sizeY, int nodeStaticCount);

private:
	// 模组对象
	OBJECT_HOLDER RouteMod* mod;

	// 工厂
	RouteFactory* factory;

	// 线路类型
	std::string type;

	// 线路名称
	std::string name;

	// 外部连接
	OBJECT_HOLDER std::vector<Node *> externs;

	// 路线节点
	OBJECT_HOLDER std::vector<Node *> nodes;

	// 路线连接
	OBJECT_HOLDER std::vector<Connection *> connections;
};

// 空线路
class EmptyRoute : public RouteMod {
public:
	/*
	* 构造空线路
	*/
	EmptyRoute();

	/*
	* 析构空线路
	*/
	virtual ~EmptyRoute();

	/*
	* 空线路静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 空线路动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 空线路实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 布局路线
	* @interfaces: 按交通类型分组的站点接口集合
	* @sizeX, sizeY: 地图尺寸
	* @nodeStaticCount: 节点静态计数起点
	*/
	virtual void LayoutRoute(
		const std::unordered_map<std::string, std::vector<std::pair<std::vector<Node*>, std::vector<std::pair<Node*, Node*>>>>>& interfaces,
		int sizeX, int sizeY, int nodeStaticCount);

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;
};
