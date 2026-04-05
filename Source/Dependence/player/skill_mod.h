#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class SkillMod {
public:
	// 无构造
	SkillMod();

	// 无析构
	virtual ~SkillMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;
};

class SkillFactory {
public:
	// 注册技能
	void RegisterSkill(const std::string& id,
		std::function<SkillMod* ()> creator, std::function<void(SkillMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建技能
	SkillMod* CreateSkill(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构技能
	void DestroySkill(SkillMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<SkillMod* ()>, std::function<void(SkillMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

