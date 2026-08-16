#include "component_mod.h"


using namespace std;

ComponentMod::ComponentMod() :
	script({ "empty", {} }) {

}

ComponentMod::~ComponentMod() {

}

ComponentFactory::ComponentFactory()
	: registries(),
	configs(),
	temp() {
	temp.registries.reserve(TEMP_RESERVE_CAPACITY);
}

void ComponentFactory::RegisterComponent(const string& id,
	function<ComponentMod* ()> creator, function<void(ComponentMod*)> deleter) {
	temp.registries[id] = { creator, deleter };
}

void ComponentFactory::MergeTemp() {
	for (auto& [id, registry] : temp.registries) {
		registries[id] = registry;
	}
}

void ComponentFactory::CleanTemp() {
	temp.registries.clear();
}

void ComponentFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

ComponentMod* ComponentFactory::CreateComponent(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Component %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Component %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Component " + id + " creater is null.\n");
	}

	return nullptr;
}

bool ComponentFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void ComponentFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void ComponentFactory::DestroyComponent(ComponentMod* componentMod) const {
	if (!componentMod) {
		debugf("Warning: Component is null when deleting.\n");
		return;
	}

	auto it = registries.find(componentMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Component %s not registered when deleting.\n", componentMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(componentMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Component " + string(componentMod->GetType()) + " deleter is null.\n");
	}
}

