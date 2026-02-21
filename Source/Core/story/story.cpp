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

Story::Story() {
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

void Story::SetResourcePath(string path) {
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
		debugf("Mod dll loaded successfully.\n");
		RegisterModEventsFunc registerFunc = (RegisterModEventsFunc)GetProcAddress(modHandle, "RegisterModEvents");
		if (registerFunc) {
			registerFunc(eventFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto eventList = { "mod" };
	for (const auto& eventId : eventList) {
		if (eventFactory->CheckRegistered(eventId)) {
			auto event = eventFactory->CreateEvent(eventId);
			debugf(("Created event: " + event->GetName() + " (ID: " + eventId + ").\n").data());
			delete event;
		}
		else {
			debugf("Event not registered: %s.\n", eventId);
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
		debugf("Mod dll loaded successfully.\n");
		RegisterModChangesFunc registerFunc = (RegisterModChangesFunc)GetProcAddress(modHandle, "RegisterModChanges");
		if (registerFunc) {
			registerFunc(changeFactory);
		}
		else {
			debugf("Incorrect dll content.\n");
		}
	}
	else {
		debugf("Failed to load mod dll.\n");
	}

#ifdef MOD_TEST
	auto changeList = { "mod" };
	for (const auto& changeId : changeList) {
		if (changeFactory->CheckRegistered(changeId)) {
			auto change = changeFactory->CreateChange(changeId);
			debugf(("Created change: " + change->GetName() + " (ID: " + changeId + ").\n").data());
			delete change;
		}
		else {
			debugf("Change not registered: %s.\n", changeId);
		}
	}
#endif // MOD_TEST

}

void Story::ReadConfigs(string path) const {
	path = resourcePath + path;
	if (!filesystem::exists(path)) {
		THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
	}

	JsonReader reader;
	JsonValue root;

	ifstream fin(path);
	if (!fin.is_open()) {
		THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
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

}

void Story::Tick(int day, int hour, int min, int sec) {

}

void Story::Print() const {

}

void Story::Load(string path) {

}

void Story::Save(string path) const {

}

void Story::ApplyChange(Change* change, Story* story,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	auto type = change->GetType();

	if (type == "set_value") {
		auto obj = dynamic_cast<SetValueChange*>(change);
		if (obj->GetVariable().substr(0, 7) == "system.") {
			return;
		}
		Condition condition;
		condition.ParseCondition(obj->GetValue());
		variables[obj->GetVariable()] = condition.EvaluateValue([this](string name) -> pair<bool, ValueType> {
			return this->GetValue(name);
			});
	}
	else if (type == "remove_value") {
		auto obj = dynamic_cast<RemoveValueChange*>(change);
		if (obj->GetVariable().substr(0, 7) == "system.") {
			return;
		}
		variables.erase(obj->GetVariable());
	}
	else if (type == "deactivate_milestone") {
		auto obj = dynamic_cast<DeactivateMilestoneChange*>(change);
		script->DeactivateMilestone(obj->GetMilestone());
	}
}

EventFactory* Story::GetEventFactory() {
	return eventFactory;
}

ChangeFactory* Story::GetChangeFactory() {
	return changeFactory;
}

vector<string> Story::ReadNames(string name, string path) const {
	if (!script) {
		THROW_EXCEPTION(StructureCrashException, "Script not initialized.\n");
	}

	return script->ReadNames(name, resourcePath + path, eventFactory, changeFactory);
}

void Story::ReadStory(string name, string path) {
	if (!script) {
		THROW_EXCEPTION(StructureCrashException, "Script not initialized.\n");
	}

	script->ReadMilestones(name, resourcePath + path, eventFactory, changeFactory);
}

bool Story::JudgeCondition(Condition& condition) const {
	return condition.EvaluateBool([this](string name) -> pair<bool, ValueType> {
		return this->GetValue(name);
		});
}

bool Story::JudgeCondition(Condition& condition, Person* person) const {
	return condition.EvaluateBool({
		[this](string name) -> pair<bool, ValueType> {
			return this->GetValue(name);
		},
		[person](string name) -> pair<bool, ValueType> {
			return person->GetValue(name);
		}
		});
}

bool Story::JudgeCondition(Condition& condition, vector<function<pair<bool, ValueType>(const string&)>> getValues) const {
	return condition.EvaluateBool(getValues);
}

pair<vector<Dialog>, vector<Change*>> Story::MatchEvent(Event* event) {
	if (!script) {
		THROW_EXCEPTION(StructureCrashException, "Script not initialized.\n");
	}

	return script->MatchEvent(event, this);
}

void Story::InitVariables(Time *t) {
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

void Story::UpdateVariables(Time *t) {
	variables["system.time.year"] = t->GetYear();
}


