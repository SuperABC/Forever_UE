// SchedulerFactory.cpp

#include "../common/error.h"
#include "../common/utility.h"

#include "scheduler_base.h"

using namespace std;

// 全成员默认构造
Scheduler::Scheduler() :
	status("home_rest") {
}

// 无析构
Scheduler::~Scheduler() {
}

// 获取状态
string Scheduler::GetStatus() const {
	return status;
}

// 设置状态
void Scheduler::SetStatus(const string& status) {
	this->status = status;
}

// 清空注册
void SchedulerFactory::RemoveAll() {
    for(auto &config : configs) {
        config.second = false;
    }
}

// 注册调度
void SchedulerFactory::RegisterScheduler(const string& id, float power,
	function<Scheduler* ()> creator, function<void(Scheduler*)> deleter) {
	registries[id] = { creator, deleter };
	powers[id] = power;
}

// 创建调度
Scheduler* SchedulerFactory::CreateScheduler(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Scheduler %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Scheduler %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.first) {
		return it->second.first();
	} else {
		THROW_EXCEPTION(NullPointerException, "Scheduler " + id + " creator is null.\n");
	}

	return nullptr;
}

// 检查是否注册
bool SchedulerFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

// 设置启用配置
void SchedulerFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

// 获取全部调度权重
const unordered_map<string, float>& SchedulerFactory::GetPowers() const {
	return powers;
}

// 析构调度
void SchedulerFactory::DestroyScheduler(Scheduler* scheduler) const {
	if (!scheduler) {
		debugf("Warning: Scheduler is null when deleting.\n");
		return;
	}

	auto it = registries.find(scheduler->GetType());
	if (it == registries.end()) {
		debugf("Warning: Scheduler %s not registered when deleting.\n", scheduler->GetType().data());
		return;
	}

	if (it->second.second) {
		it->second.second(scheduler);
	} else {
		THROW_EXCEPTION(NullPointerException, "Scheduler " + scheduler->GetType() + " deleter is null.\n");
	}
}

