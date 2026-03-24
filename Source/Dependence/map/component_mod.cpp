#include "component_mod.h"


using namespace std;

ComponentMod::ComponentMod() {

}

ComponentMod::~ComponentMod() {

}

void ComponentFactory::RegisterComponent(const string& id,
	function<ComponentMod* ()> creator, function<void(ComponentMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void ComponentFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
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

void ComponentFactory::DestroyComponent(ComponentMod* Component) const {
	if (!Component) {
		debugf("Warning: Component is null when deleting.\n");
		return;
	}

	auto it = registries.find(Component->GetType());
	if (it == registries.end()) {
		debugf("Warning: Component %s not registered when deleting.\n", Component->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Component);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Component " + string(Component->GetType()) + " deleter is null.\n");
	}
}

