#include "script_mod.h"

#include "dialog.h"
#include "change.h"


using namespace std;

ScriptMod::ScriptMod() :
	scriptPath(),
	actionQueue(),
	wrapping(false) {

}

ScriptMod::~ScriptMod() {

}

bool ScriptMod::EnableWrapping() const {
	return false;
}

void ScriptMod::WrapScript(const Event* event, const vector<ReadOnlyScriptAction>& actions,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues,
	PostHandle* post) {

}

void ScriptMod::DeepCopy(const vector<ReadOnlyScriptAction>& actions) {
	for (auto& action : actions) {
		visit([this](auto* ptr) {
			using T = decay_t<decltype(ptr)>;
			if constexpr (is_same_v<T, const Dialog*>) {
				actionQueue.push_back(ptr);
			}
			else if constexpr (is_same_v<T, const Change*>) {
				actionQueue.push_back(ptr->Clone());
			}
		}, action);
	}
}

void ScriptMod::AutoClean() {
	for (auto& action : actionQueue) {
		visit([](auto* ptr) {
			using T = decay_t<decltype(ptr)>;
			if constexpr (is_same_v<T, const Change*>) {
				delete ptr;
			}
		}, action);
	}
	actionQueue.clear();
}

void ScriptFactory::RegisterScript(const string& id, bool main,
	function<ScriptMod* ()> creator, function<void(ScriptMod*)> deleter) {
	registries[id] = { creator, deleter };
	if (main) {
		if (this->main.size() > 0) {
			debugf("Warning: Multiple main script loaded, overwriting the previous one.\n");
		}
		this->main = id;
	}
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

string ScriptFactory::GetMain() {
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

