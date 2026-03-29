#pragma once

#include "story/script.h"

#include "scheduler_mod.h"


// 子类注册函数
typedef void (*RegisterModSchedulersFunc)(SchedulerFactory* factory);

// 调度实体
class Scheduler {
public:
	// 禁止默认构造
	Scheduler() = delete;

	// 通过类型从工厂构造
	Scheduler(SchedulerFactory* factory, const std::string& component);

	// 析构调度
	~Scheduler();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 初始化调度
	void InitScheduler();

private:
	// 模组对象
	OBJECT_HOLDER SchedulerMod* mod;

	// 工厂
	SchedulerFactory* factory;

	// 调度类型
	std::string type;

	// 调度名称
	std::string name;

	// 关联剧情
	OBJECT_HOLDER Script* script;
};

// 空调度
class EmptyScheduler : public SchedulerMod {
public:
	EmptyScheduler();
	virtual ~EmptyScheduler();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static float GetPower();

	virtual void InitScheduler();

private:
	static int count;

	int id;
	std::string name;
};

