#include "story.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

EventFactory* Story::eventFactory = nullptr;
ChangeFactory* Story::changeFactory = nullptr;

Story::Story() :
	resourcePath(),
	script(nullptr),
	variables() {
	if (!eventFactory) {
		eventFactory = new EventFactory();
	}
	if (!changeFactory) {
		changeFactory = new ChangeFactory();
	}
}

Story::~Story() {
	delete script;
}

void Story::SetResourcePath(const string& path) {
	resourcePath = path;
}

void Story::InitEvents(unordered_map<string, HMODULE>& modHandles) {
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
		RegisterModEventsFunc registerFunc =
			(RegisterModEventsFunc)GetProcAddress(modHandle, "RegisterModEvents");
		if (registerFunc) {
			registerFunc(eventFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto eventList = { "mod" };
	for (const auto& eventId : eventList) {
		if (eventFactory->CheckRegistered(eventId)) {
			auto event = eventFactory->CreateEvent(eventId);
			debugf("Log: Created test event %s.\n", eventId);
			eventFactory->DestroyEvent(event);
		}
		else {
			debugf("Warning: Event %s not registered.\n", eventId);
		}
	}
#endif // MOD_TEST
}

void Story::InitChanges(unordered_map<string, HMODULE>& modHandles) {
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
		RegisterModChangesFunc registerFunc =
			(RegisterModChangesFunc)GetProcAddress(modHandle, "RegisterModChanges");
		if (registerFunc) {
			registerFunc(changeFactory);
		}
		else {
			debugf("Warning: Incorrect dll content.\n");
		}
	}
	else {
		debugf("Warning: Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto changeList = { "mod" };
	for (const auto& changeId : changeList) {
		if (changeFactory->CheckRegistered(changeId)) {
			auto change = changeFactory->CreateChange(changeId);
			debugf("Log: Created test change %s.\n", changeId);
			changeFactory->DestroyChange(change);
		}
		else {
			debugf("Warning: Change %s not registered.\n", changeId);
		}
	}
#endif // MOD_TEST
}

void Story::ReadConfigs(const string& path) const {
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
		for (auto event : root["mods"]["event"]) {
			eventFactory->SetConfig(event.AsString(), true);
		}
		for (auto change : root["mods"]["change"]) {
			changeFactory->SetConfig(change.AsString(), true);
		}
	}
	else {
		fin.close();
		THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
	}
	fin.close();
}

void Story::Init() {
	script = new Script();
}

void Story::Destroy() {
	// 保留空实现
}

void Story::Tick(int day, int hour, int min, int sec) {
	// 保留空实现
}

void Story::Print() const {
	// 保留空实现
}

void Story::Load(const string& path) {
	// 保留空实现
}

void Story::Save(const string& path) const {
	// 保留空实现
}

void Story::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (change == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Change is null.\n");
	}

	auto type = change->GetType();

	if (type == "set_value") {
		auto obj = dynamic_cast<SetValueChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to SetValueChange.\n");
		}
		if (obj->GetVariable().substr(0, 7) == "system.") {
			return;
		}
		Condition condition;
		condition.ParseCondition(obj->GetValue());
		variables[obj->GetVariable()] = condition.EvaluateValue(
			[this](string name) -> pair<bool, ValueType> {
				return this->GetValue(name);
			});
	}
	else if (type == "remove_value") {
		auto obj = dynamic_cast<RemoveValueChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to RemoveValueChange.\n");
		}
		if (obj->GetVariable().substr(0, 7) == "system.") {
			return;
		}
		variables.erase(obj->GetVariable());
	}
	else if (type == "deactivate_milestone") {
		auto obj = dynamic_cast<DeactivateMilestoneChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to DeactivateMilestoneChange.\n");
		}
		script->DeactivateMilestone(obj->GetMilestone());
	}
}

EventFactory* Story::GetEventFactory() const {
	return eventFactory;
}

ChangeFactory* Story::GetChangeFactory() const {
	return changeFactory;
}

vector<string> Story::ReadNames(const string& name, const string& path) const {
	if (script == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Script not initialized.\n");
	}

	return script->ReadNames(name, resourcePath + path,
		eventFactory, changeFactory);
}

void Story::ReadStory(const string& name, const string& path) {
	if (script == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Script not initialized.\n");
	}

	script->ReadMilestones(name, resourcePath + path,
		eventFactory, changeFactory);
}

bool Story::JudgeCondition(const Condition& condition) const {
	return condition.EvaluateBool([this](string name) -> pair<bool, ValueType> {
		return this->GetValue(name);
		});
}

bool Story::JudgeCondition(const Condition& condition,
	const Person* person) const {
	if (person == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Person pointer is null.\n");
	}
	return condition.EvaluateBool({
		[this](string name) -> pair<bool, ValueType> {
			return this->GetValue(name);
		},
		[person](string name) -> pair<bool, ValueType> {
			return person->GetValue(name);
		}
		});
}

bool Story::JudgeCondition(const Condition& condition,
	vector<function<pair<bool, ValueType>(const string&)>> getValues) const {
	return condition.EvaluateBool(getValues);
}

pair<vector<Dialog>, vector<Change*>> Story::MatchEvent(Event* event) {
	if (script == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Script not initialized.\n");
	}
	if (event == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Event is null.\n");
	}

	return script->MatchEvent(event, this);
}

void Story::InitVariables(Time* t) {
	if (t == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Time is null.\n");
	}
	variables["player.name"] = "玩家";
	variables["player.health"] = "健康";
	variables["system.time.year"] = t->GetYear();
}

void Story::SetValue(const string& name, ValueType value) {
	variables[name] = value;
}

pair<bool, ValueType> Story::GetValue(const string& name) const {
	auto it = variables.find(name);
	if (it != variables.end()) {
		return { true, it->second };
	}
	return { false, 0 };
}

void Story::UpdateVariables(Time* t) {
	if (t == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Time is null.\n");
	}
	variables["system.time.year"] = t->GetYear();
}

