#include "job_base.h"
#include "../common/error.h"
#include "../common/utility.h"


using namespace std;

Job::Job() :
    calendar(nullptr),
    position(nullptr) {
    // 全成员默认构造

}

Job::~Job() {
    // calendar/position在map中统一创建和析构

}

Calendar* Job::GetCalendar() const {
    // 获取工作日历
    return calendar;
}

void Job::SetCalendar(Calendar* calendar) {
    // 设置工作日历
    this->calendar = calendar;
}

Room* Job::GetPosition() const {
    // 获取工作房间
    return position;
}

void Job::SetPosition(Room* room) {
    // 设置工作房间
    position = room;
}

void JobFactory::RegisterJob(const string& id,
    function<Job* ()> creator, function<void(Job*)> deleter) {
    // 注册构造器和析构器
    registries[id] = { creator, deleter };
}

Job* JobFactory::CreateJob(const string& id) {
    // 根据配置构造职业
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

bool JobFactory::CheckRegistered(const string& id) const {
    // 检查是否注册
    return registries.find(id) != registries.end();
}

void JobFactory::SetConfig(const string& name, bool config) {
    // 设置启用配置
    configs[name] = config;
}

void JobFactory::DestroyJob(Job* job) const {
    // 析构职业
    if (!job) return;
    auto it = registries.find(job->GetType());
    if (it != registries.end()) {
        it->second.second(job);
    }
    else {
        debugf(("Deleter not found for " + job->GetType() + ".\n").data());
    }
}