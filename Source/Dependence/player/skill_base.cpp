#include "../common/error.h"
#include "../common/utility.h"

#include "skill_base.h"


using namespace std;

// 无构造
Skill::Skill() {

}

// 无析构
Skill::~Skill() {
    
}

// 注册技能
void SkillFactory::RegisterSkill(const string& id,
    function<Skill*()> creator, function<void(Skill*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建技能
Skill* SkillFactory::CreateSkill(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Skill %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Skill %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Skill " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool SkillFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void SkillFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 获取所有启用的技能
vector<Skill*> SkillFactory::GetSkills() const {
    vector<Skill*> skills;
    for (const auto& r : registries) {
        if (configs.find(r.first) != configs.end() && configs.at(r.first)) {
            skills.push_back(r.second.first());
        }
    }
    return skills;
}

// 析构技能
void SkillFactory::DestroySkill(Skill* skill) const {
    if (!skill) {
        debugf("Warning: Skill is null when deleting.\n");
        return;
    }

    auto it = registries.find(skill->GetType());
    if (it == registries.end()) {
        debugf("Warning: Skill %s not registered when deleting.\n", skill->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(skill);
    } else {
        THROW_EXCEPTION(NullPointerException, "Skill " + skill->GetType() + " deleter is null.\n");
    }
}

