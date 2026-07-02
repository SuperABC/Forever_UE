#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


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

	// 关联剧情与脚本
	std::pair<std::string, std::vector<std::string>> script;
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


