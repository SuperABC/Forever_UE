#include "route_mod.h"


using namespace std;

RouteMod::RouteMod() {

}

RouteMod::~RouteMod() {

}

void RouteFactory::RegisterRoute(const string& id,
	function<RouteMod* ()> creator, function<void(RouteMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void RouteFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
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

void RouteFactory::DestroyRoute(RouteMod* Route) const {
	if (!Route) {
		debugf("Warning: Route is null when deleting.\n");
		return;
	}

	auto it = registries.find(Route->GetType());
	if (it == registries.end()) {
		debugf("Warning: Route %s not registered when deleting.\n", Route->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Route);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Route " + string(Route->GetType()) + " deleter is null.\n");
	}
}

