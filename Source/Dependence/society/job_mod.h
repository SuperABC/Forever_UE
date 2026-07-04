#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include "../story/change.h"

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>


class Script;


class JobMod {
public:
	/*
	* 无构造
	*/
	JobMod();

	/*
	* 无析构
	*/
	virtual ~JobMod();

	/*
	* Override
	* 职业静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 职业动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 职业实例唯一名称
	*/
	virtual const char* GetName() = 0; 

	/*
	* Override
	* 初始化职业
	*/
	COSTOM_INIT virtual void InitJob() = 0;

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
	* @storyScript, schedulerScript, jobScript: 主线剧情脚本、持有该职业的市民调度脚本、职业自身剧情脚本
	*/
	COSTOM_RUNTIME virtual void ExecNode(const std::string& name,
		Container* storyScript, Container* schedulerScript, Container* jobScript) = 0;

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;

	// 每日计划（节点名 -> 触发时刻）
	std::unordered_map<std::string, Time> plans;

	// 节点执行产出的变化列表
	std::vector<ChangeValue> changes;
};

class JobFactory {
public:
	/*
	* 注册职业
	* @id: 职业静态类型标识
	* @creator, deleter: 构造与析构方法
	*/
	void RegisterJob(const std::string& id,
		std::function<JobMod* ()> creator, std::function<void(JobMod*)> deleter);

	/*
	* 清空注册
	*/
	void RemoveAll();

	/*
	* 创建职业
	* @id: 职业静态类型标识
	*/
	JobMod* CreateJob(const std::string& id) const;

	/*
	* 检查是否注册
	* @id: 职业静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 职业静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构职业
	* @jobMod: 待析构的职业模组对象
	*/
	void DestroyJob(JobMod* jobMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<JobMod* ()>, std::function<void(JobMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};


