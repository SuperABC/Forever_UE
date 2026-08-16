#include "calendar_mod.h"


using namespace std;

CalendarMod::CalendarMod() {

}

CalendarMod::~CalendarMod() {

}

CalendarFactory::CalendarFactory()
	: registries(),
	configs(),
	temp() {
	temp.registries.reserve(TEMP_RESERVE_CAPACITY);
}

void CalendarFactory::RegisterCalendar(const string& id,
	function<CalendarMod* ()> creator, function<void(CalendarMod*)> deleter) {
	temp.registries[id] = { creator, deleter };
}

void CalendarFactory::MergeTemp() {
	for (auto& [id, registry] : temp.registries) {
		registries[id] = registry;
	}
}

void CalendarFactory::CleanTemp() {
	temp.registries.clear();
}

void CalendarFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
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

void CalendarFactory::DestroyCalendar(CalendarMod* calendarMod) const {
	if (!calendarMod) {
		debugf("Warning: Calendar is null when deleting.\n");
		return;
	}

	auto it = registries.find(calendarMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Calendar %s not registered when deleting.\n", calendarMod->GetType());
		return;
	}

	if (it->second.second) {
		it->second.second(calendarMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Calendar " + string(calendarMod->GetType()) + " deleter is null.\n");
	}
}

