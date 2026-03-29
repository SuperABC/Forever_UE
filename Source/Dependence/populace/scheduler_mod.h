#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class SchedulerMod {
public:
	// 无构造
	SchedulerMod();

	// 无析构
	virtual ~SchedulerMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 调度分配权重
	COSTOM_INIT static float GetPower();

	// 初始化调度
	COSTOM_INIT virtual void InitScheduler() = 0;
	
	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
};

class SchedulerFactory {
public:
	// 注册调度
	void RegisterScheduler(const std::string& id, float power,
		std::function<SchedulerMod* ()> creator, std::function<void(SchedulerMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建调度
	SchedulerMod* CreateScheduler(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取全部调度权重
	std::unordered_map<std::string, float> GetPowers() const;

	// 析构调度
	void DestroyScheduler(SchedulerMod* terrain) const;

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

