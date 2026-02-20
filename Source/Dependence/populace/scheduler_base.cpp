#include "scheduler_base.h"


using namespace std;

Scheduler::Scheduler() {
    status = "home_rest";
}

Scheduler::~Scheduler() {

}

string Scheduler::GetStatus() {
    return status;
}

void Scheduler::SetStatus(string status) {
    this->status = status;
}

void SchedulerFactory::RegisterScheduler(const string& id, function<Scheduler* ()> creator, float power) {
    registries[id] = creator;
    powers[id] = power;
}

Scheduler* SchedulerFactory::CreateScheduler(const string& id) {
    if(configs.find(id) == configs.end() || !configs.find(id)->second)return nullptr;
    
    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second();
    }
    return nullptr;
}

bool SchedulerFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void SchedulerFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

const unordered_map<string, float>& SchedulerFactory::GetPowers() const {
    return powers;
}
