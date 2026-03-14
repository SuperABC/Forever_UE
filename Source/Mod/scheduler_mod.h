#pragma once

#include "../common/error.h"
#include "../common/utility.h"

#include "scheduler_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModScheduler : public Scheduler {
public:
	ModScheduler() {}
	~ModScheduler() {}

	static std::string GetId() { return "mod"; }
	virtual std::string GetType() const override { return "mod"; }
	virtual std::string GetName() const override { return "模组调度"; }

	static float GetPower() {
		return 1.0f;
	}

};

