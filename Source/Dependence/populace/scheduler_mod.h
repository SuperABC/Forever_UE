#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "../story/change.h"

#include <string>
#include <functional>
#include <vector>


class Script;


class SchedulerMod {
public:
	/*
	* 无构造
	*/
	SchedulerMod();

	/*
	* 无析构
	*/
	virtual ~SchedulerMod();

	/*
	* Override
	* 调度静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 调度动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 调度实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 调度分配权重
	*/
	COSTOM_INIT static float GetPower();

	/*
	* Override
	* 初始化调度
	*/
	COSTOM_INIT virtual void InitScheduler() = 0;

	/*
	* Override
	* 每日规划，填充plans字段
	* @time: 当前游戏时间
	*/
	COSTOM_RUNTIME virtual void DailyPlan(const Time& time) = 0;

	/*
	* Override
	* 规划节点调用，结果写入changes字段
	* @name: 节点名称
	* @storyScript, schedulerScript: 主线剧情脚本与调度自身的剧情脚本
	* @jobScripts: 市民所有工作的剧情脚本列表
	*/
	COSTOM_RUNTIME virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* schedulerScript, const std::vector<Container*>& jobScripts) = 0;

	/*
	* 添加一条计划
	* @name: 节点名称
	* @time: 触发时刻
	*/
	void AddNode(const std::string& name, const Time& time);

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;

	// 每日计划（节点名 -> 触发时刻）
	std::unordered_map<std::string, Time> plans;

	// 节点执行产出的变化列表
	std::vector<ChangeValue> changes;
};

class SchedulerFactory {
public:
	/*
	* 注册调度
	* @id: 调度静态类型标识
	* @power: 分配权重
	* @creator, deleter: 构造与析构方法
	*/
	void RegisterScheduler(const std::string& id, float power,
		std::function<SchedulerMod* ()> creator, std::function<void(SchedulerMod*)> deleter);

	/*
	* 清空注册
	*/
	void RemoveAll();

	/*
	* 创建调度
	* @id: 调度静态类型标识
	*/
	SchedulerMod* CreateScheduler(const std::string& id) const;

	/*
	* 检查是否注册
	* @id: 调度静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 调度静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取全部调度权重
	*/
	std::unordered_map<std::string, float> GetPowers() const;

	/*
	* 析构调度
	* @schedulerMod: 待析构的调度模组对象
	*/
	void DestroyScheduler(SchedulerMod* schedulerMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<SchedulerMod* ()>, std::function<void(SchedulerMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 权重
	std::unordered_map<std::string, float> powers;
};

