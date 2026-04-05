#pragma once

#include "skill_mod.h"


// 读心技能
class InfomationSkill : public SkillMod {
public:
	InfomationSkill();
	virtual ~InfomationSkill();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

private:
	static int count;

	int id;
	std::string name;
};
