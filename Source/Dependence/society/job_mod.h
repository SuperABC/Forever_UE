#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class JobMod {
public:
	// 无构造
	JobMod();

	// 无析构
	virtual ~JobMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 初始化职业
	virtual void InitJob() = 0;

	// 工作日程
	std::string calendar;

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
};

class JobFactory {
public:
	// 注册组合
	void RegisterJob(const std::string& id,
		std::function<JobMod* ()> creator, std::function<void(JobMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建组合
	JobMod* CreateJob(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构组合
	void DestroyJob(JobMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<JobMod* ()>, std::function<void(JobMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};


