#include "event_base.h"
#include "../common/error.h"
#include "../common/utility.h"

using namespace std;

void Event::SetCondition(Condition condition) {
    this->condition = condition;
}

Condition& Event::GetCondition() {
    return condition;
}

void EventFactory::RegisterEvent(const string& id,
    function<Event*()> creator, function<void(Event*)> deleter) {
    registries[id] = {creator, deleter};
    configs[id] = false;
}

Event* EventFactory::CreateEvent(const string& id) {
    auto it = registries.find(id);
    if (it != registries.end()) {
        return it->second.first();
    }
    return nullptr;
}

bool EventFactory::CheckRegistered(const string& id) {
    return registries.find(id) != registries.end();
}

void EventFactory::SetConfig(string name, bool config) {
    if (configs.find(name) != configs.end()) {
        configs[name] = config;
    }
}

void EventFactory::DestroyEvent(Event* event) const {
    if (!event) return;
    auto it = registries.find(event->GetType());
    if (it != registries.end()) {
        it->second.second(event);
    } else {
        debugf(("Deleter not found for " + event->GetType() + ".\n").data());
    }
}