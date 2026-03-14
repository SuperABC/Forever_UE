#pragma once

#include "../common/error.h"
#include "../common/utility.h"

#include "component_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModComponent : public Component {
public:
	ModComponent() : Component() {}
	~ModComponent() {}

	static std::string GetId() { return "mod"; }
	virtual std::string GetType() const override { return "mod"; }
	virtual std::string GetName() const override { return "模组组合"; }
};