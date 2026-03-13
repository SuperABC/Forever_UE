#include "../common/error.h"
#include "../common/utility.h"

#include "scheduler_base.h"


using namespace std;

Scheduler::Scheduler() :
	status("home_rest") {
	// 全成员默认构造

}

Scheduler::~Scheduler() {
	// 无析构

}

string Scheduler::GetStatus() const {
	// 获取状态
	return status;
}

void Scheduler::SetStatus(const string& status) {
	// 设置状态
	this->status = status;
}

void SchedulerFactory::RegisterScheduler(const string& id, float power,
	function<Scheduler* ()> creator, function<void(Scheduler*)> deleter) {
	// 注册构造器和析构器
	registries[id] = { creator, deleter };
	powers[id] = power;
}

Scheduler* SchedulerFactory::CreateScheduler(const string& id) const {
	// 根据配置构造调度
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

bool SchedulerFactory::CheckRegistered(const string& id) const {
	// 检查是否注册
	return registries.find(id) != registries.end();
}

void SchedulerFactory::SetConfig(const string& name, bool config) {
	// 设置启用配置
	configs[name] = config;
}

const unordered_map<string, float>& SchedulerFactory::GetPowers() const {
	// 获取全部调度权重
	return powers;
}

void SchedulerFactory::DestroyScheduler(Scheduler* scheduler) const {
	// 析构调度
	if (!scheduler) {
		debugf("Scheduler is null before destroying.\n");
		return;
	}
	auto it = registries.find(scheduler->GetType());
	if (it != registries.end()) {
		it->second.second(scheduler);
	}
	else {
		debugf("Deleter not found for %s.\n", scheduler->GetType().data());
	}
}

