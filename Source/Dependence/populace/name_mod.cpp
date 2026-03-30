#include "name_mod.h"


using namespace std;

NameMod::NameMod() {

}

NameMod::~NameMod() {

}

void NameFactory::RegisterName(const string& id,
	function<NameMod* ()> creator, function<void(NameMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void NameFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

NameMod* NameFactory::CreateName(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Name %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Name %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Name " + id + " creater is null.\n");
	}

	return nullptr;
}

bool NameFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void NameFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

string NameFactory::GetName() const {
	for (auto [name, enabled] : configs) {
		if (enabled) {
			return name;
		}
	}
	return "";
}

void NameFactory::DestroyName(NameMod* Name) const {
	if (!Name) {
		debugf("Warning: Name is null when deleting.\n");
		return;
	}

	auto it = registries.find(Name->GetType());
	if (it == registries.end()) {
		debugf("Warning: Name %s not registered when deleting.\n", Name->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Name);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Name " + string(Name->GetType()) + " deleter is null.\n");
	}
}

