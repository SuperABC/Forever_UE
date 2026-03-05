#include "calendar_base.h"
#include "../common/error.h"
#include "../common/utility.h"

using namespace std;

void CalendarFactory::RegisterCalendar(const string& id,
    function<Calendar*()> creator, function<void(Calendar*)> deleter) {
    registries[id] = {creator, deleter};
}

Calendar* CalendarFactory::CreateCalendar(const string& id) {
    if (configs.find(id) == configs.end() || !configs.find(id)->second)
        return nullptr;

    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool CalendarFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void CalendarFactory::SetConfig(string name, bool config) {
    configs[name] = config;
}

void CalendarFactory::DestroyCalendar(Calendar* calendar) const {
    if (!calendar) return;
    auto it = registries.find(calendar->GetType());
    if (it != registries.end()) {
        it->second.second(calendar);
    } else {
        debugf(("Deleter not found for " + calendar->GetType() + ".\n").data());
    }
}
