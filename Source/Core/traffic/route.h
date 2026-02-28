#pragma once

#include "route_base.h"

#include <string>
#include <vector>


// 子类注册函数
typedef void (*RegisterModRoutesFunc)(RouteFactory* factory);

// 飞机航线
class AirRoute : public Route {
public:
	static std::string GetId();
	std::string GetType() const override;
	std::string GetName() const override;

	bool StandAlone() const override;
	bool RouteVisible() const override;
};