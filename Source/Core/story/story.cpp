#include "story.h"


using namespace std;

ScriptFactory* Story::scriptFactory = nullptr;

Story::Story() :
	script(nullptr) {
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

			auto registerFunc = (RegisterModScriptsFunc)GetProcAddress(modHandle, "RegisterModScripts");
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

	for (auto [name, zone] : map->GetZones()) {
		auto setup = zone->GetScriptSetup();
		auto script = new Script(scriptFactory, setup.first);
		for (auto s : setup.second) {
			script->ReadMilestones(Config::GetScript(s));
		}
		script->SetValue("self.name", name);
		zone->SetScript(script);
		for (auto [name, building] : zone->GetBuildings()) {
			auto setup = building->GetScriptSetup();
			auto script = new Script(scriptFactory, setup.first);
			for (auto s : setup.second) {
				script->ReadMilestones(Config::GetScript(s));
			}
			script->SetValue("self.name", name);
			building->SetScript(script);
		}
	}
	for (auto [name, building] : map->GetBuildings()) {
		auto setup = building->GetScriptSetup();
		auto script = new Script(scriptFactory, setup.first);
		for (auto s : setup.second) {
			script->ReadMilestones(Config::GetScript(s));
		}
		script->SetValue("self.name", name);
		building->SetScript(script);
	}
	for (auto citizen : populace->GetCitizens()) {
		auto setup = citizen->GetScheduler()->GetScriptSetup();
		auto script = new Script(scriptFactory, setup.first);
		for (auto s : setup.second) {
			script->ReadMilestones(Config::GetScript(s));
		}
		script->SetValue("self.name", citizen->GetName());
		citizen->GetScheduler()->SetScript(script);
	}
}

void Story::Destroy() {
	if(script)delete script;
	script = nullptr;
}

void Story::ApplyChange(Change* change,
	std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) {
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
		script->SetValue(obj->GetVariable(), condition.EvaluateValue(getValues));
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
		script->DeactivateMilestone(obj->GetMilestone());
	}
}

Script* Story::GetScript() const {
	return script;
}