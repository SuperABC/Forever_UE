#include "app_mod.h"


using namespace std;

AppMod::AppMod() {

}

AppMod::~AppMod() {

}

void AppFactory::RegisterApp(const string& id,
	function<void()> init, function<int(Canvas*, int)> loop,
	function<void(Canvas*)> back, function<void(Canvas*)> refresh,
	function<AppMod*()> creator, function<void(AppMod*)> deleter) {
	registries[id] = { init, loop, back, refresh, creator, deleter };
}

void AppFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

AppMod* AppFactory::CreateApp(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: App %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: App %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.creator) {
		return it->second.creator();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "App " + id + " creator is null.\n");
	}

	return nullptr;
}

bool AppFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void AppFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void AppFactory::InitApp(const string& id) const {
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: App %s not registered when initializing.\n", id.data());
		return;
	}

	if (it->second.init) {
		it->second.init();
	}
	else {
		debugf("Warning: App %s has no init function.\n", id.data());
	}
}

int AppFactory::LoopApp(const string& id, Canvas* canvas, int ms) const {
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: App %s not registered when looping.\n", id.data());
		return 0;
	}

	if (it->second.loop) {
		return it->second.loop(canvas, ms);
	}
	else {
		debugf("Warning: App %s has no loop function.\n", id.data());
		return 0;
	}
}

void AppFactory::BackApp(const string& id, Canvas* canvas) const {
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: App %s not registered when backing.\n", id.data());
		return;
	}
	if (it->second.back) {
		it->second.back(canvas);
	}
}

void AppFactory::RefreshApp(const string& id, Canvas* canvas) const {
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: App %s not registered when refreshing.\n", id.data());
		return;
	}
	if (it->second.refresh) {
		it->second.refresh(canvas);
	}
}

void AppFactory::DestroyApp(AppMod* appMod) const {
	if (!appMod) {
		debugf("Warning: App is null when deleting.\n");
		return;
	}

	auto it = registries.find(appMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: App %s not registered when deleting.\n", appMod->GetType());
		return;
	}

	if (it->second.deleter) {
		it->second.deleter(appMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "App " + string(appMod->GetType()) + " deleter is null.\n");
	}
}
