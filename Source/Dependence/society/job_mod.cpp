#include "job_mod.h"


using namespace std;

JobMod::JobMod():
	script() {

}

JobMod::~JobMod() {

}

void JobFactory::RegisterJob(const string& id,
	function<JobMod* ()> creator, function<void(JobMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void JobFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

JobMod* JobFactory::CreateJob(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Job %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Job %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Job " + id + " creater is null.\n");
	}

	return nullptr;
}

bool JobFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void JobFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void JobFactory::DestroyJob(JobMod* Job) const {
	if (!Job) {
		debugf("Warning: Job is null when deleting.\n");
		return;
	}

	auto it = registries.find(Job->GetType());
	if (it == registries.end()) {
		debugf("Warning: Job %s not registered when deleting.\n", Job->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Job);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Job " + string(Job->GetType()) + " deleter is null.\n");
	}
}


