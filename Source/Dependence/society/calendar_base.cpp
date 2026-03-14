#include "../common/error.h"
#include "../common/utility.h"

#include "calendar_base.h"


using namespace std;

Calendar::Calendar() {
	// 无构造

}

Calendar::~Calendar() {
	// 无析构

}

void CalendarFactory::RegisterCalendar(const string& id,
	function<Calendar* ()> creator, function<void(Calendar*)> deleter) {
	// 注册构造器和析构器
	registries[id] = { creator, deleter };
}

Calendar* CalendarFactory::CreateCalendar(const string& id) {
	// 根据配置构造日程
	auto configIt = configs.find(id);
	if (configIt == configs.end() || !configIt->second) {
		return nullptr;
	}

	auto it = registries.find(id);
	if (it != registries.end()) {
		return it->second.first();
	}
	return nullptr;
}

bool CalendarFactory::CheckRegistered(const string& id) {
	// 检查是否注册
	return registries.find(id) != registries.end();
}

void CalendarFactory::SetConfig(const string& name, bool config) {
	// 设置启用配置
	configs[name] = config;
}

void CalendarFactory::DestroyCalendar(Calendar* calendar) const {
	// 析构日程
	if (!calendar) {
		return;
	}
	auto it = registries.find(calendar->GetType());
	if (it != registries.end()) {
		it->second.second(calendar);
	}
	else {
		debugf(("Deleter not found for " + calendar->GetType() + ".\n").data());
	}
}

