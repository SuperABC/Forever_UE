#include "player.h"
#include "utility.h"
#include "error.h"
#include "json.h"
#include "config.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

SkillFactory* Player::skillFactory = nullptr;

Player::Player() :
	time(nullptr),
	skills() {
	if (!skillFactory) {
		skillFactory = new SkillFactory();
	}
	time = new Time();
}

Player::~Player() {
	delete time;
}

void Player::InitSkills(unordered_map<string, HMODULE>& modHandles,
	vector<string>& dlls) {
	skillFactory->RegisterSkill(DefaultSkill::GetId(),
		[]() { return new DefaultSkill(); },
		[](Skill* skill) { delete skill; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = (RegisterModSkillsFunc)GetProcAddress(modHandle, "RegisterModSkills");
			if (registerFunc) {
				registerFunc(skillFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}

#ifdef MOD_TEST
	auto skillList = { "mod" };
	for (const auto& skillId : skillList) {
		if (skillFactory->CheckRegistered(skillId)) {
			auto skill = skillFactory->CreateSkill(skillId);
			debugf("Log: Created test skill %s.\n", skillId);
			skillFactory->DestroySkill(skill);
		}
		else {
			debugf("Warning: Skill %s not registered.\n", skillId);
		}
	}
#endif // MOD_TEST
}

void Player::LoadConfigs() const {
	skillFactory->RemoveAll();

	auto skills = Config::GetEnables("skill");
	for (auto skill : skills) {
		skillFactory->SetConfig(skill, true);
	}
}

void Player::Init() {
	skills = skillFactory->GetSkills();
}

void Player::Destroy() {

}

void Player::Tick(int day, int hour, int min, int sec) {

}

void Player::Print() const {

}

void Player::Load(const string& path) {

}

void Player::Save(const string& path) const {

}

void Player::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (change == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Change pointer is null in ApplyChange.\n");
	}
	if (story == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Story pointer is null in ApplyChange.\n");
	}

}

Time* Player::GetTime() {
	return time;
}

