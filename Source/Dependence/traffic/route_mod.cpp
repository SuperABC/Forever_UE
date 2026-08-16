#include "route_mod.h"


using namespace std;

RouteMod::RouteMod() :
	externs(),
	nodes(),
	connections() {

}

RouteMod::~RouteMod() {

}

RouteFactory::RouteFactory()
	: registries(),
	configs(),
	temp() {
	temp.registries.reserve(TEMP_RESERVE_CAPACITY);
}

void RouteFactory::RegisterRoute(const string& id,
	function<RouteMod* ()> creator, function<void(RouteMod*)> deleter) {
	temp.registries[id] = { creator, deleter };
}

void RouteFactory::MergeTemp() {
	for (auto& [id, registry] : temp.registries) {
		registries[id] = registry;
	}
}

void RouteFactory::CleanTemp() {
	temp.registries.clear();
}

void RouteFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

RouteMod* RouteFactory::CreateRoute(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Route %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Route %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Route " + id + " creater is null.\n");
	}

	return nullptr;
}

bool RouteFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void RouteFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

vector<string> RouteFactory::GetRoutes() const {
	vector<string> routes;
	for (auto& registry : registries) {
		auto config = configs.find(registry.first);
		if (config != configs.end() && config->second) {
			routes.push_back(registry.first);
		}
	}
	return routes;
}

void RouteFactory::DestroyRoute(RouteMod* routeMod) const {
	if (!routeMod) {
		debugf("Warning: Route is null when deleting.\n");
		return;
	}

	auto it = registries.find(routeMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Route %s not registered when deleting.\n", routeMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(routeMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Route " + string(routeMod->GetType()) + " deleter is null.\n");
	}
}

