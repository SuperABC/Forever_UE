#include "name_base.h"
//#include "job_base.h"
//#include "scheduler_base.h"


using namespace std;

bool Name::ReserveName(std::string name) {
    if (reserve.find(name) == reserve.end()) {
        reserve.insert(name);
        return true;
    }
    return false;
}

bool Name::UseName(std::string name) {
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

void NameFactory::RegisterName(const string& id, function<Name*()> creator) {
    registries[id] = creator;
}

Name* NameFactory::CreateName(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
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
    for (auto config : configs) {
        if (config.second)return registries.find(config.first)->second();
    }
    return nullptr;
}
