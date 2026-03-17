#include "../common/error.h"
#include "../common/utility.h"

#include "job_base.h"


using namespace std;

// 全成员默认构造
Job::Job() :
    calendar(nullptr),
    position(nullptr) {

}

// calendar/position在map中统一创建和析构
Job::~Job() {

}

// 获取工作日历
Calendar* Job::GetCalendar() const {
    return calendar;
}

// 设置工作日历
void Job::SetCalendar(Calendar* calendar) {
    this->calendar = calendar;
}

// 获取工作房间
Room* Job::GetPosition() const {
    return position;
}

// 设置工作房间
void Job::SetPosition(Room* room) {
    position = room;
}

// 清空注册
void JobFactory::RemoveAll() {
    for(auto &config : configs) {
        config.second = false;
    }
}

// 注册工作
void JobFactory::RegisterJob(const string& id,
    function<Job* ()> creator, function<void(Job*)> deleter) {
    registries[id] = { creator, deleter };
}

// 创建工作
Job* JobFactory::CreateJob(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Job %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Job %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Job " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool JobFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void JobFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构工作
void JobFactory::DestroyJob(Job* job) const {
    if (!job) {
        debugf("Warning: Job is null when deleting.\n");
        return;
    }

    auto it = registries.find(job->GetType());
    if (it == registries.end()) {
        debugf("Warning: Job %s not registered when deleting.\n", job->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(job);
    } else {
        THROW_EXCEPTION(NullPointerException, "Job " + job->GetType() + " deleter is null.\n");
    }
}

