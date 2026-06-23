#pragma once

#include "populace/scheduler_mod.h"


// 纯工作调度
class WorkonlyScheduler : public SchedulerMod {
public:
	WorkonlyScheduler();
	virtual ~WorkonlyScheduler();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static float GetPower();

	virtual void InitScheduler() override;

private:
	static int count;

	int id;
	std::string name;
};

