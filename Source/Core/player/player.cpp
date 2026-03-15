#include "player.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

SkillFactory* Player::skillFactory = nullptr;

Player::Player() :
	resourcePath(),
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

void Player::SetResourcePath(const string& path) {
	resourcePath = path;
}

void Player::InitSkills(unordered_map<string, HMODULE>& modHandles) {
	skillFactory->RegisterSkill(DefaultSkill::GetId(),
		[]() { return new DefaultSkill(); },
		[](Skill* skill) { delete skill; }
	);

	string modPath = "Mod.dll";
	HMODULE modHandle;
	if (modHandles.find(modPath) != modHandles.end()) {
		modHandle = modHandles[modPath];
	}
	else {
		modHandle = LoadLibraryA(modPath.data());
		modHandles[modPath] = modHandle;
	}
	if (modHandle) {
		debugf("Log: Mod dll loaded successfully.\n");
		RegisterModSkillsFunc registerFunc =
			(RegisterModSkillsFunc)GetProcAddress(modHandle, "RegisterModSkills");
		if (registerFunc) {
			registerFunc(skillFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
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

void Player::Init() {
	skills = skillFactory->GetSkills();
}

void Player::ReadConfigs(const string& path) const {
	string fullPath = resourcePath + path;
	if (!filesystem::exists(fullPath)) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + fullPath + ".\n");
	}

	JsonReader reader;
	JsonValue root;

	ifstream fin(fullPath);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + fullPath + ".\n");
	}
	if (reader.Parse(fin, root)) {
		for (auto skill : root["mods"]["skill"]) {
			skillFactory->SetConfig(skill.AsString(), true);
		}
	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
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

