#pragma once

#include "populace/scheduler_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModSchedulersFunc)(SchedulerFactory* factory);

// 调度实体
class Scheduler {
public:
	/*
	* 禁止默认构造
	*/
	Scheduler() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 调度工厂
	* @scheduler: 调度静态类型标识
	*/
	Scheduler(SchedulerFactory* factory, const std::string& scheduler);

	/*
	* 析构调度
	*/
	~Scheduler();

	/*
	* 获取类型
	*/
	std::string GetType() const;

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 初始化调度
	* @name: 持有该调度的市民姓名
	*/
	void InitScheduler(std::string name);

	/*
	* 获取剧本
	*/
	Script* GetScript() const;

	/*
	* 每日计划
	*/
	void DailyPlan();

	/*
	* 执行节点，返回节点计算出的变化列表（由调用方负责释放）
	* @node: 节点名称
	* @storyScript: 主线剧情脚本，用于节点内的变量解析；调度自身的脚本由内部获取
	* @jobScripts: 该市民所有工作的剧情脚本列表
	*/
	std::vector<Change*> ExecNode(const std::string& node, Script* storyScript, const std::vector<Script*>& jobScripts);

	/*
	* 获取今日计划（节点名 -> 目标时刻）
	*/
	const std::unordered_map<std::string, Time>& GetPlans() const;

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
	/*
	* 构造空调度
	*/
	EmptyScheduler();

	/*
	* 析构空调度
	*/
	virtual ~EmptyScheduler();

	/*
	* Override
	* 调度静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 调度动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 调度实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 调度分配权重
	*/
	static float GetPower();

	/*
	* Override
	* 初始化调度
	*/
	virtual void InitScheduler() override;

	/*
	* Override
	* 每日规划
	*/
	virtual void DailyPlan() override;

	/*
	* Override
	* 规划节点调用
	* @node: 节点名称
	* @storyScript, schedulerScript: 主线剧情脚本与调度自身的剧情脚本
	* @jobScripts: 市民所有工作的剧情脚本列表
	*/
	virtual void ExecNode(const std::string& node,
		Script* storyScript, Script* schedulerScript, const std::vector<Script*>& jobScripts) override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;
};

