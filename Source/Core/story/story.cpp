#include "story.h"

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
	timerSet() {
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
}

void Story::Destroy() {
	if(script)delete script;
	script = nullptr;
}

void Story::Tick(Player* player) {
	currentTime = *player->GetTime();

	script->SetValue("system.time.year", currentTime.GetYear());
	script->SetValue("system.time.month", currentTime.GetMonth());
	script->SetValue("system.time.day", currentTime.GetDay());
	script->SetValue("system.time.hour", currentTime.GetHour());
	script->SetValue("system.time.minute", currentTime.GetMinute());
	script->SetValue("system.time.second", currentTime.GetSecond());
}

void Story::ApplyChange(Change* change,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues,
	Script* targetScript) {
	if (change == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Change is null.\n");
	}

	auto type = change->GetType();

	if (type == "game_end") {
		auto obj = dynamic_cast<GameEndChange*>(change);
		script->ClearContext();
	}
	else if (type == "set_value") {
		auto obj = dynamic_cast<SetValueChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to SetValueChange.\n");
		}
		if (obj->GetVariable().substr(0, 7) == "system." ||
			obj->GetVariable().substr(0, 7) == "self." ||
			obj->GetVariable().substr(0, 7) == "local.") {
			return;
		}
		Condition conditionVariable;
		conditionVariable.ParseCondition(obj->GetVariable());
		Condition conditionValue;
		conditionValue.ParseCondition(obj->GetValue());
		script->SetValue(ToString(conditionVariable.EvaluateValue(getValues)),
			conditionValue.EvaluateValue(getValues));
	}
	else if (type == "remove_value") {
		auto obj = dynamic_cast<RemoveValueChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to RemoveValueChange.\n");
		}
		if (obj->GetVariable().substr(0, 7) == "system.") {
			return;
		}
		script->RemoveValue(obj->GetVariable());
	}
	else if (type == "deactivate_milestone") {
		auto obj = dynamic_cast<DeactivateMilestoneChange*>(change);
		if (obj == nullptr) {
			THROW_EXCEPTION(RuntimeException, "Failed to cast Change to DeactivateMilestoneChange.\n");
		}
		Script* target = targetScript ? targetScript : script;
		target->DeactivateMilestone(obj->GetMilestone());
	}
	else if (type == "create_timer") {
		auto obj = dynamic_cast<CreateTimerChange*>(change);
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

