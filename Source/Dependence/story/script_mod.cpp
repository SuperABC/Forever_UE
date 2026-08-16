#include "script_mod.h"

#include "dialog.h"
#include "change.h"


using namespace std;

ScriptMod::ScriptMod() :
	scriptPath(),
	actionStack() {

}

ScriptMod::~ScriptMod() {

}

void ScriptMod::WrapScript(const Event* event, const vector<ScriptAction>& actions,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues,
	PostHandle* post) {
	AutoCopy(actions);
}

void ScriptMod::AutoPop() {
	actionStack.pop_back();
}

void ScriptMod::AutoCopy(const vector<ScriptAction>& actions) {
	actionStack.emplace_back(actions.begin(), actions.end());
}

int ScriptMod::FindLabel(const string& label) const {
	if (actionStack.empty()) {
		return -1;
	}

	auto& top = actionStack.back();
	for (size_t i = 0; i < top.size(); i++) {
		if (auto* change = get_if<const Change*>(&top[i])) {
			auto* placeholder = dynamic_cast<const PlaceHolderChange*>(*change);
			if (placeholder && placeholder->GetLabel() == label) {
				return static_cast<int>(i);
			}
		}
	}
	return -1;
}

ScriptFactory::ScriptFactory()
	: registries(),
	configs(),
	main(),
	temp() {
	temp.registries.reserve(TEMP_RESERVE_CAPACITY);
}

void ScriptFactory::RegisterScript(const string& id, bool main,
	function<ScriptMod* ()> creator, function<void(ScriptMod*)> deleter) {
	temp.registries[id] = { creator, deleter };
	if (main) {
		if (temp.main.size() > 0) {
			debugf("Warning: Multiple main script loaded, overwriting the previous one.\n");
		}
		temp.main = id;
	}
}

void ScriptFactory::MergeTemp() {
	for (auto& [id, registry] : temp.registries) {
		registries[id] = registry;
	}
	if (!temp.main.empty()) {
		main = temp.main;
	}
}

void ScriptFactory::CleanTemp() {
	temp.registries.clear();
	temp.main.clear();
}

void ScriptFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

ScriptMod* ScriptFactory::CreateScript(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Script %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Script %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Script " + id + " creater is null.\n");
	}

	return nullptr;
}

bool ScriptFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void ScriptFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

string ScriptFactory::GetMain() const {
	return main;
}

void ScriptFactory::DestroyScript(ScriptMod* scriptMod) const {
	if (!scriptMod) {
		debugf("Warning: Script is null when deleting.\n");
		return;
	}

	auto it = registries.find(scriptMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Script %s not registered when deleting.\n", scriptMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(scriptMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Script " + string(scriptMod->GetType()) + " deleter is null.\n");
	}
}

