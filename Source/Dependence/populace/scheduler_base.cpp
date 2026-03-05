#include "scheduler_base.h"
#include "../common/error.h"
#include "../common/utility.h"

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

void SchedulerFactory::RegisterScheduler(const string& id, float power,
                                         function<Scheduler*()> creator, function<void(Scheduler*)> deleter) {
    registries[id] = {creator, deleter};
    powers[id] = power;
}

Scheduler* SchedulerFactory::CreateScheduler(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
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

void SchedulerFactory::DestroyScheduler(Scheduler* scheduler) const {
    if(!scheduler)return;
    auto it = registries.find(scheduler->GetType());
    if (it != registries.end()) {
        it->second.second(scheduler);
    } else {
        debugf(("Deleter not found for " + scheduler->GetType() + ".\n").data());
    }
}