#include "script_mod.h"


using namespace std;

ScriptMod::ScriptMod() :
    scriptPath(),
    actionQueue() {

}

ScriptMod::~ScriptMod() {

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
    for (auto& config : configs) {
        config.second = false;
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

std::string ScriptFactory::GetMain() {
    return main;
}

void ScriptFactory::DestroyScript(ScriptMod* terrain) const {
    if (!terrain) {
        debugf("Warning: Script is null when deleting.\n");
        return;
    }

    auto it = registries.find(terrain->GetType());
    if (it == registries.end()) {
        debugf("Warning: Script %s not registered when deleting.\n", terrain->GetType());
        return;
    }

    if (it->second.second) {
        it->second.second(terrain);
    }
    else {
        THROW_EXCEPTION(NullPointerException, "Script " + string(terrain->GetType()) + " deleter is null.\n");
    }
}

