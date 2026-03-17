#include "../common/error.h"
#include "../common/utility.h"

#include "event_base.h"


using namespace std;

// 全成员默认构造
Event::Event() :
    condition() {

}

// 无析构
Event::~Event() {

}

// 获取控制条件
Condition& Event::GetCondition() {
    return condition;
}

// 设置控制条件
void Event::SetCondition(const Condition& condition) {
    this->condition = condition;
}

// 清空注册
void EventFactory::RemoveAll() {
    for(auto &config : configs) {
        config.second = false;
    }
}

// 注册事件
void EventFactory::RegisterEvent(const string& id,
    function<Event*()> creator, function<void(Event*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建事件
Event* EventFactory::CreateEvent(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Event %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Event %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Event " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool EventFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void EventFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构事件
void EventFactory::DestroyEvent(Event* event) const {
    if (!event) {
        debugf("Warning: Event is null when deleting.\n");
        return;
    }

    auto it = registries.find(event->GetType());
    if (it == registries.end()) {
        debugf("Warning: Event %s not registered when deleting.\n", event->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(event);
    } else {
        THROW_EXCEPTION(NullPointerException, "Event " + event->GetType() + " deleter is null.\n");
    }
}

