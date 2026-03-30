#include "scheduler_mod.h"


using namespace std;

SchedulerMod::SchedulerMod() :
	script({ "empty", {} }) {

}

SchedulerMod::~SchedulerMod() {

}

void SchedulerFactory::RegisterScheduler(const string& id, float power,
	function<SchedulerMod* ()> creator, function<void(SchedulerMod*)> deleter) {
	registries[id] = { creator, deleter };
	powers[id] = power;
}

void SchedulerFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

SchedulerMod* SchedulerFactory::CreateScheduler(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Scheduler %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Scheduler %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Scheduler " + id + " creater is null.\n");
	}

	return nullptr;
}

bool SchedulerFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void SchedulerFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

unordered_map<string, float> SchedulerFactory::GetPowers() const {
	auto result = powers;
	for (auto& [name, power] : powers) {
		if (configs.find(name) == configs.end() || !configs.at(name)) {
			result.erase(name);
		}
	}
	return result;
}

void SchedulerFactory::DestroyScheduler(SchedulerMod* Scheduler) const {
	if (!Scheduler) {
		debugf("Warning: Scheduler is null when deleting.\n");
		return;
	}

	auto it = registries.find(Scheduler->GetType());
	if (it == registries.end()) {
		debugf("Warning: Scheduler %s not registered when deleting.\n", Scheduler->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Scheduler);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Scheduler " + string(Scheduler->GetType()) + " deleter is null.\n");
	}
}

