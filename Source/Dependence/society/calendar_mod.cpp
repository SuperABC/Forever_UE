#include "calendar_mod.h"


using namespace std;

CalendarMod::CalendarMod() {

}

CalendarMod::~CalendarMod() {

}

void CalendarFactory::RegisterCalendar(const string& id,
	function<CalendarMod* ()> creator, function<void(CalendarMod*)> deleter) {
	registries[id] = { creator, deleter };
}

void CalendarFactory::RemoveAll() {
	for (auto& config : configs) {
		config.second = false;
	}
}

CalendarMod* CalendarFactory::CreateCalendar(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Calendar %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Calendar %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Calendar " + id + " creater is null.\n");
	}

	return nullptr;
}

bool CalendarFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void CalendarFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void CalendarFactory::DestroyCalendar(CalendarMod* Calendar) const {
	if (!Calendar) {
		debugf("Warning: Calendar is null when deleting.\n");
		return;
	}

	auto it = registries.find(Calendar->GetType());
	if (it == registries.end()) {
		debugf("Warning: Calendar %s not registered when deleting.\n", Calendar->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(Calendar);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Calendar " + string(Calendar->GetType()) + " deleter is null.\n");
	}
}

