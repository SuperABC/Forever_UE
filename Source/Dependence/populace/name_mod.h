#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class NameMod {
public:
	// 无构造
	NameMod();

	// 无析构
	virtual ~NameMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 从姓名中获取姓
	COSTOM_RUNTIME virtual void GetSurname(std::function<void(const std::string&)> set,
		const std::string& name) = 0;

	// 全随机生成姓名
	COSTOM_RUNTIME virtual void GenerateName(std::function<void(const std::string&)> set,
		bool male = true, bool female = true, bool neutral = true) = 0;

	// 给定姓随机生成名
	COSTOM_RUNTIME virtual void GenerateName(std::function<void(const std::string&)> set,
		const std::string& surname, bool male = true, bool female = true, bool neutral = true) = 0;
};

class NameFactory {
public:
	// 注册姓名
	void RegisterName(const std::string& id,
		std::function<NameMod* ()> creator, std::function<void(NameMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建姓名
	NameMod* CreateName(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构姓名
	void DestroyName(NameMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<NameMod* ()>, std::function<void(NameMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

