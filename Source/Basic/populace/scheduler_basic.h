#pragma once

#include "populace/scheduler_mod.h"


// 纯工作调度
class WorkonlyScheduler : public SchedulerMod {
public:
	/*
	* 构造纯工作调度
	*/
	WorkonlyScheduler();

	/*
	* 析构纯工作调度
	*/
	virtual ~WorkonlyScheduler();

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
	* @time: 当前游戏时间
	*/
	virtual void DailyPlan(const Time& time) override;

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

