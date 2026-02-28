#include "skill_base.h"


using namespace std;

void SkillFactory::RegisterSkill(const string& id, function<Skill* ()> creator) {
    registries[id] = creator;
}

Skill* SkillFactory::CreateSkill(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
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
        if (configs.find(r.first)->second) {
            skills.push_back(r.second());
        }
    }
    return skills;
}