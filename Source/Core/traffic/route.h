#pragma once

#include "traffic/route_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModRoutesFunc)(RouteFactory* factory);

// 线路实体
class Route {
public:
	// 禁止默认构造
	Route() = delete;

	// 通过类型从工厂构造
	Route(RouteFactory* factory, const std::string& route);

	// 析构线路
	~Route();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

private:
	// 模组对象
	OBJECT_HOLDER RouteMod* mod;

	// 工厂
	RouteFactory* factory;

	// 线路类型
	std::string type;

	// 线路名称
	std::string name;

};

// 空线路
class EmptyRoute : public RouteMod {
public:
	EmptyRoute();
	virtual ~EmptyRoute();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

