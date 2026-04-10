#include "player.h"

#include "player/skill.h"


using namespace std;

SkillFactory* Player::skillFactory = nullptr;

Player::Player() :
	time(nullptr) {
	time = new Time();
	time->SetHour(8);

	if (!skillFactory) {
		skillFactory = new SkillFactory();
	}
}

Player::~Player() {
	delete time;

	Destroy();
}

void Player::LoadConfigs() const {
	skillFactory->RemoveAll();

	auto skills = Config::GetEnables("skill");
	for (auto skill : skills) {
		skillFactory->SetConfig(skill, true);
	}
}

void Player::InitSkills(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	skillFactory->RegisterSkill(EmptySkill::GetId(),
		[]() { return new EmptySkill(); },
		[](SkillMod* skill) { delete skill; }
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
}

void Player::Init() {
	Destroy();
}

void Player::Destroy() {
	for (auto& skill : skills) {
		if (skill)delete skill;
		skill = nullptr;
	}
	skills.clear();
}

void Player::Tick(float delta) {
	time->AddMilliseconds((int)(delta * 60 * 1000 * 60));
}

void Player::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) {

}

Time* Player::GetTime() {
	return time;
}
