#include "story.h"

#include "map/map.h"
#include "map/zone.h"
#include "map/building.h"
#include "story/script.h"
#include "story/event.h"
#include "story/dialog.h"
#include "story/change.h"
#include "player/player.h"


using namespace std;

ScriptFactory* Story::scriptFactory = nullptr;

Story::Story() :
	script(nullptr),
	historyTalk(),
	currentTime(),
	timerSet(),
	pendingMessages(),
	globalSettings() {
	if (!scriptFactory) {
		scriptFactory = new ScriptFactory();
	}
}

Story::~Story() {
	Destroy();
}

void Story::LoadConfigs() const {
	scriptFactory->RemoveAll();

	auto scripts = Config::GetEnables("script");
	for (auto script : scripts) {
		scriptFactory->SetConfig(script, true);
	}
}

void Story::InitScripts(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	scriptFactory->RegisterScript(EmptyScript::GetId(), EmptyScript::MainStory(),
		[]() { return new EmptyScript(); },
		[](ScriptMod* script) { delete script; }
	);
	scriptFactory->MergeTemp();
	scriptFactory->CleanTemp();

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

			auto registerFunc = reinterpret_cast<RegisterModScriptsFunc>(GetProcAddress(modHandle, "RegisterModScripts"));
			if (registerFunc) {
				registerFunc(scriptFactory);
				scriptFactory->MergeTemp();

				auto finishFunc = reinterpret_cast<FinishModScriptsFunc>(GetProcAddress(modHandle, "FinishModScripts"));
				if (finishFunc) {
					finishFunc(scriptFactory);
				}
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Story::Init(Map* map, Populace* populace, Player* player) {
	Destroy();

	script = new Script(scriptFactory, scriptFactory->GetMain());
	for (auto story : Config::GetStories()) {
		script->ReadMilestones(story);
	}
	script->SetValue("system.time.year", player->GetTime()->GetYear());

	globalSettings = Config::GetGlobalSettings();
}

void Story::Destroy() {
	if(script)delete script;
	script = nullptr;
}

void Story::Tick(Player* player) {
	currentTime = *player->GetTime();
	if (!script)return;

	script->SetValue("system.time.year", currentTime.GetYear());
	script->SetValue("system.time.month", currentTime.GetMonth());
	script->SetValue("system.time.day", currentTime.GetDay());
	script->SetValue("system.time.hour", currentTime.GetHour());
	script->SetValue("system.time.minute", currentTime.GetMinute());
	script->SetValue("system.time.second", currentTime.GetSecond());
}

vector<Event*> Story::ApplyChange(const Change* change,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues,
	Script* targetScript) {
	vector<Event*> result;
	if (change == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Change is null.\n");
	}

	auto type = change->GetType();

	if (type == "game_end") {
		auto obj = dynamic_cast<const GameEndChange*>(change);
		script->ClearContext();
	}
	else if (type == "set_value") {
		auto obj = dynamic_cast<const SetValueChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to SetValueChange.\n");
		}
		if (obj->GetVariable().substr(0, 7) == "system." ||
			obj->GetVariable().substr(0, 5) == "self." ||
			obj->GetVariable().substr(0, 6) == "local.") {
			return result;
		}
		Condition conditionVariable;
		conditionVariable.ParseCondition(obj->GetVariable());
		Condition conditionValue;
		conditionValue.ParseCondition(obj->GetValue());
		script->SetValue(ToString(conditionVariable.EvaluateValue(getValues)),
			conditionValue.EvaluateValue(getValues));
	}
	else if (type == "global_setting") {
		auto obj = dynamic_cast<const GlobalSettingChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to GlobalSettingChange.\n");
		}
		Condition settingCondition;
		settingCondition.ParseCondition(obj->GetSetting());
		Condition valueCondition;
		valueCondition.ParseCondition(obj->GetValue());
		globalSettings[ToString(settingCondition.EvaluateValue(getValues))] =
			valueCondition.EvaluateValue(getValues);
	}
	else if (type == "remove_value") {
		auto obj = dynamic_cast<const RemoveValueChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to RemoveValueChange.\n");
		}
		if (obj->GetVariable().substr(0, 7) == "system.") {
			return result;
		}
		script->RemoveValue(obj->GetVariable());
	}
	else if (type == "deactivate_milestone") {
		auto obj = dynamic_cast<const DeactivateMilestoneChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to DeactivateMilestoneChange.\n");
		}
		Script* target = targetScript ? targetScript : script;
		target->DeactivateMilestone(obj->GetMilestone());
	}
	else if (type == "create_timer") {
		auto obj = dynamic_cast<const CreateTimerChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to CreateTimerChange.\n");
		}
		Condition nameCondition;
		nameCondition.ParseCondition(obj->GetName());
		string name = ToString(nameCondition.EvaluateValue(getValues));

		Condition timeCondition;
		timeCondition.ParseCondition(obj->GetTime());
		string time = ToString(timeCondition.EvaluateValue(getValues));

		Condition labelCondition;
		labelCondition.ParseCondition(obj->GetLabel());
		string label = ToString(labelCondition.EvaluateValue(getValues));

		CreateTimer(name, Time(time), obj->GetCategory(), label);
	}
	else if (type == "remove_timer") {
		auto obj = dynamic_cast<const RemoveTimerChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to RemoveTimerChange.\n");
		}
		Condition nameCondition;
		nameCondition.ParseCondition(obj->GetName());
		string name = ToString(nameCondition.EvaluateValue(getValues));

		RemoveTimer(name);
	}
	return result;
}

Script* Story::GetScript() const {
	return script;
}

void Story::AddTalk(const string& speaker, const string& content) {
	historyTalk.push_back({ speaker, content });
}

const vector<pair<string, string>>& Story::GetHistory() const {
	return historyTalk;
}

void Story::CreateTimer(const string& name, const Time& time, const string& category, const string& label) {
	Time target = currentTime;
	target.SetTime(time.GetHour(), time.GetMinute(), time.GetSecond(), time.GetMillisecond());
	if (time.GetOnlySecond() <= currentTime.GetOnlySecond()) {
		target.AddDays(1);
	}
	timerSet.insert({ target, name, category, label });
}

void Story::RemoveTimer(const string& name) {
	for (auto it = timerSet.begin(); it != timerSet.end(); ++it) {
		if (get<1>(*it) == name) {
			timerSet.erase(it);
			break;
		}
	}
}

vector<tuple<string, string, string>> Story::PopExpiredTimers(const Time& now, int maxCount) {
	vector<tuple<string, string, string>> result;

	while (static_cast<int>(result.size()) < maxCount && !timerSet.empty()) {
		auto it = timerSet.begin();
		auto& [target, name, category, label] = *it;
		if (now < target) break;
		result.emplace_back(name, category, label);
		timerSet.erase(it);
	}

	return result;
}

void Story::AddMessage(const string& message) {
	pendingMessages.push_back(message);
}

vector<string> Story::PopMessages() {
	vector<string> result;
	result.swap(pendingMessages);
	return result;
}

Room* Story::GetCurrentRoom(Map* map) const {
	auto zone = map->GetZone(ToString(script->GetValue("system.player.zone").second));
	if (zone) {
		auto building = zone->GetBuilding(ToString(script->GetValue("system.player.building").second));
		if (!building) return nullptr;
		return building->GetRoom(ToString(script->GetValue("system.player.room").second));
	}
	else {
		auto building = map->GetBuilding(ToString(script->GetValue("system.player.building").second));
		if (!building) return nullptr;
		return building->GetRoom(ToString(script->GetValue("system.player.room").second));
	}

	return nullptr;
}

ValueType Story::GetSetting(const string& setting) const {
	auto it = globalSettings.find(setting);
	if (it == globalSettings.end()) {
		THROW_EXCEPTION(RuntimeException, "Unknown global setting: " + setting + ".\n");
	}
	return it->second;
}

Script* Story::CreateLocal(Event* event,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (!event) return nullptr;

	Script* local = nullptr;

	if (auto* e = dynamic_cast<PuzzleResultEvent*>(event)) {
		local = new Script(Story::scriptFactory, "empty");
		local->SetValue("local.result", e->GetResult());
	}
	else if (auto* e = dynamic_cast<OptionDialogEvent*>(event)) {
		local = new Script(Story::scriptFactory, "empty");
		local->SetValue("local.name", e->GetName());
		local->SetValue("local.option", e->GetOption());
	}
	else if (auto* e = dynamic_cast<ObjectResultEvent*>(event)) {
		local = new Script(Story::scriptFactory, "empty");
		local->SetValue("local.result", e->GetResult());
		local->SetValue("local.num", e->GetNum());
	}

	if (local) {
		getValues.push_back([local](const string& name) -> pair<bool, ValueType> {
			return local->GetValue(name);
		});
	}
	return local;
}
