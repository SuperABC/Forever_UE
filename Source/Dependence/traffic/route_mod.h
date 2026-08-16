#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../map/geometry.h"

#include <string>
#include <functional>


// 路线Mod基类
class RouteMod {
public:
	/*
	* 构造路线
	*/
	RouteMod();

	/*
	* 析构路线
	*/
	virtual ~RouteMod();

	/*
	* Override
	* 路线静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 路线动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 路线实例唯一名称
	* @return: 实例唯一名称
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
	* 构造路线工厂,预留暂存区容量
	*/
	RouteFactory();

	/*
	* 注册路线
	* @id: 路线类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterRoute(const std::string& id,
		std::function<RouteMod* ()> creator, std::function<void(RouteMod*)> deleter);

	/*
	* 将暂存数据合并进正式注册表
	*/
	void MergeTemp();

	/*
	* 清空暂存数据
	*/
	void CleanTemp();

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建路线
	* @id: 路线类型
	*/
	RouteMod* CreateRoute(const std::string& id) const;

	/*
	* 检查路线是否已注册
	* @id: 路线类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置路线启用配置
	* @name: 路线类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取所有已启用的线路类型
	*/
	std::vector<std::string> GetRoutes() const;

	/*
	* 析构路线
	* @routeMod: 待析构的路线
	*/
	void DestroyRoute(RouteMod* routeMod) const;

private:
	// 暂存本轮注册写入的数据,避免跨模块直接写正式成员
	struct Temp {
		// 注册表
		std::unordered_map<
			std::string,
			std::pair<std::function<RouteMod* ()>, std::function<void(RouteMod*)>>
		> registries;
	};

	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<RouteMod* ()>, std::function<void(RouteMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 暂存数据
	Temp temp;
};
