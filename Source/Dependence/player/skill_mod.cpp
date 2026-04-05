#include "skill_mod.h"


using namespace std;

SkillMod::SkillMod() {

}

SkillMod::~SkillMod() {

}

void SkillFactory::RegisterSkill(const string& id,
	function<SkillMod* ()> creator, function<void(SkillMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void SkillFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

SkillMod* SkillFactory::CreateSkill(const string& id) const {
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
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Skill " + id + " creater is null.\n");
	}

	return nullptr;
}

bool SkillFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void SkillFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void SkillFactory::DestroySkill(SkillMod* Skill) const {
	if (!Skill) {
		debugf("Warning: Skill is null when deleting.\n");
		return;
	}

	auto it = registries.find(Skill->GetType());
	if (it == registries.end()) {
		debugf("Warning: Skill %s not registered when deleting.\n", Skill->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Skill);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Skill " + string(Skill->GetType()) + " deleter is null.\n");
	}
}

