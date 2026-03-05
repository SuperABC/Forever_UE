#include "skill_base.h"
#include "../common/error.h"
#include "../common/utility.h"

using namespace std;

void SkillFactory::RegisterSkill(const string& id,
    function<Skill*()> creator, function<void(Skill*)> deleter) {
    registries[id] = {creator, deleter};
}

Skill* SkillFactory::CreateSkill(const string& id) {
    if (configs.find(id) == configs.end() || !configs.at(id))
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first(); // 调用 creator
    }
    return nullptr;
}

bool SkillFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void SkillFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

vector<Skill*> SkillFactory::GetSkills() {
    vector<Skill*> skills;
    for (const auto& r : registries) {
        // 检查 config 是否存在且为 true
        if (configs.find(r.first) != configs.end() && configs.at(r.first)) {
            skills.push_back(r.second.first());
        }
    }
    return skills;
}

void SkillFactory::DestroySkill(Skill* skill) const {
    if (!skill) return;
    auto it = registries.find(skill->GetType());
    if (it != registries.end()) {
        it->second.second(skill);
    } else {
        debugf(("Deleter not found for " + skill->GetType() + ".\n").data());
    }
}