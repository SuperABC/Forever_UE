#pragma once

#include "../common/utility.h"
#include "../common/error.h"

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

	// 析构线路
	void DestroyRoute(RouteMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<RouteMod* ()>, std::function<void(RouteMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

