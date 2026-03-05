#include "name_base.h"
#include "../common/error.h"
#include "../common/utility.h"

using namespace std;

bool Name::ReserveName(string name) {
    if (reserve.find(name) == reserve.end()) {
        reserve.insert(name);
        return true;
    }
    return false;
}

bool Name::UseName(string name) {
    if (reserve.find(name) != reserve.end()) {
        reserve.erase(name);
        roll.insert(name);
        return true;
    }
    return false;
}

bool Name::RegisterName(string name) {
    if (roll.find(name) == roll.end() && reserve.find(name) == reserve.end()) {
        roll.insert(name);
        return true;
    }
    return false;
}

void NameFactory::RegisterName(const string& id,
                               function<Name*()> creator,
                               function<void(Name*)> deleter) {
    registries[id] = {creator, deleter};
}

Name* NameFactory::CreateName(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool NameFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void NameFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

Name* NameFactory::GetName() const {
    for (const auto& config : configs) {
        if (config.second) {
            auto it = registries.find(config.first);
            if (it != registries.end()) {
                return it->second.first();
            }
        }
    }
    return nullptr;
}

void NameFactory::DestroyName(Name* name) const {
    if(!name)return;
    auto it = registries.find(name->GetType());
    if (it != registries.end()) {
        it->second.second(name);
    } else {
        debugf(("Deleter not found for " + name->GetType() + ".\n").data());
    }
}