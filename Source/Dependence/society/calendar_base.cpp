#include "../common/error.h"

#include "calendar_base.h"


using namespace std;

// 无构造
Calendar::Calendar() {

}

// 无析构
Calendar::~Calendar() {

}

// 注册日程
void CalendarFactory::RegisterCalendar(const string& id,
    function<Calendar* ()> creator, function<void(Calendar*)> deleter) {
    registries[id] = { creator, deleter };
}

// 创建日程
Calendar* CalendarFactory::CreateCalendar(const string& id) const {
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
    } else {
        THROW_EXCEPTION(NullPointerException, "Calendar " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool CalendarFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void CalendarFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构日程
void CalendarFactory::DestroyCalendar(Calendar* calendar) const {
    if (!calendar) {
        debugf("Warning: Calendar is null when deleting.\n");
        return;
    }

    auto it = registries.find(calendar->GetType());
    if (it == registries.end()) {
        debugf("Warning: Calendar %s not registered when deleting.\n", calendar->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(calendar);
    } else {
        THROW_EXCEPTION(NullPointerException, "Calendar " + calendar->GetType() + " deleter is null.\n");
    }
}

