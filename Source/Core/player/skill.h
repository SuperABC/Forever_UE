#pragma once

#include "skill_mod.h"


// 子类注册函数
typedef void (*RegisterModSkillsFunc)(SkillFactory* factory);

// 技能实体
class Skill {
public:
	// 禁止默认构造
	Skill() = delete;

	// 通过类型从工厂构造
	Skill(SkillFactory* factory, const std::string& component);

	// 析构技能
	~Skill();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

private:
	// 模组对象
	OBJECT_HOLDER SkillMod* mod;

	// 工厂
	SkillFactory* factory;

	// 技能类型
	std::string type;

	// 技能名称
	std::string name;

};

// 空技能
class EmptySkill : public SkillMod {
public:
	EmptySkill();
	virtual ~EmptySkill();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};

