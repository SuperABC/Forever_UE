#pragma once

#include "route_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModRoute : public Route {
public:
    static std::string GetId() { return "mod"; }
    virtual std::string GetType() const override { return "mod"; }
    virtual std::string GetName() const override { return "模组线路"; }

	bool StandAlone() const override { return true; }
	bool RouteVisible() const override { return false; }
};

