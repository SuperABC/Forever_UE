#include "organization_mod.h"


using namespace std;

OrganizationMod::OrganizationMod() {

}

OrganizationMod::~OrganizationMod() {

}

void OrganizationFactory::RegisterOrganization(const string& id, float power,
	function<OrganizationMod* ()> creator, function<void(OrganizationMod*)> deleter) {
	registries[id] = { creator, deleter };
	powers[id] = power;
}

void OrganizationFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

OrganizationMod* OrganizationFactory::CreateOrganization(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Organization %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Organization %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Organization " + id + " creater is null.\n");
	}

	return nullptr;
}

bool OrganizationFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void OrganizationFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

unordered_map<string, float> OrganizationFactory::GetPowers() const {
	auto result = powers;
	for (auto& [name, power] : powers) {
		if (configs.find(name) == configs.end() || !configs.at(name)) {
			result.erase(name);
		}
	}
	return result;
}

void OrganizationFactory::DestroyOrganization(OrganizationMod* Organization) const {
	if (!Organization) {
		debugf("Warning: Organization is null when deleting.\n");
		return;
	}

	auto it = registries.find(Organization->GetType());
	if (it == registries.end()) {
		debugf("Warning: Organization %s not registered when deleting.\n", Organization->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Organization);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Organization " + string(Organization->GetType()) + " deleter is null.\n");
	}
}

