#pragma once

#include "skill_base.h"

#include <string>
#include <vector>


// 子类注册函数
typedef void (*RegisterModSkillsFunc)(SkillFactory* factory);

// 标准双休日程
class DefaultSkill : public Skill {
public:
    static std::string GetId();
    virtual std::string GetType() const override;
    virtual std::string GetName() const override;

};