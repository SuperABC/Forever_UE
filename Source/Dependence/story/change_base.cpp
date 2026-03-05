#include "change_base.h"
#include "../common/error.h"
#include "../common/utility.h"

using namespace std;

void Change::SetCondition(Condition condition) {
    this->condition = condition;
}

Condition& Change::GetCondition() {
    return condition;
}

void ChangeFactory::RegisterChange(const string& id,
                                   function<Change*()> creator,
                                   function<void(Change*)> deleter) {
    registries[id] = {creator, deleter};
    configs[id] = false;
}

Change* ChangeFactory::CreateChange(const string& id) {
    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool ChangeFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void ChangeFactory::SetConfig(string name, bool config) {
    if (configs.find(name) != configs.end()) {
        configs[name] = config;
    }
}

void ChangeFactory::DestroyChange(Change* change) const {
    if (!change) return;
    auto it = registries.find(change->GetType());
    if (it != registries.end()) {
        it->second.second(change);
    } else {
        debugf(("Deleter not found for " + change->GetType() + ".\n").data());
    }
}