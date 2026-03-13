#include "../common/error.h"
#include "../common/utility.h"

#include "name_base.h"


using namespace std;

Name::Name() :
	reserve(),
	roll() {
	// 全成员默认构造

}

Name::~Name() {
	// 无析构

}

bool Name::ReserveName(const string& name) {
	// 占用名字
	if (reserve.find(name) == reserve.end()) {
		reserve.insert(name);
		return true;
	}
	return false;
}

bool Name::RegisterName(const string& name) {
	// 使用名字
	if (roll.find(name) == roll.end() && reserve.find(name) == reserve.end()) {
		roll.insert(name);
		return true;
	}
	return false;
}

void NameFactory::RegisterName(const string& id,
	function<Name* ()> creator, function<void(Name*)> deleter) {
	// 注册构造器和析构器
	registries[id] = { creator, deleter };
}

Name* NameFactory::CreateName(const string& id) {
	// 根据配置构造姓名
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		return nullptr;
	}

	auto it = registries.find(id);
	if (it != registries.end()) {
		return it->second.first();
	}
	return nullptr;
}

bool NameFactory::CheckRegistered(const string& id) {
	// 检查是否注册
	return registries.find(id) != registries.end();
}

void NameFactory::SetConfig(const string& name, bool config) {
	// 设置启用配置
	configs[name] = config;
}

Name* NameFactory::GetName() const {
	// 获取启用姓名
	for (const auto& config : configs) {
		if (config.second) {
			auto it = registries.find(config.first);
			if (it != registries.end()) {
				return it->second.first();
			}
		}
	}
	return nullptr;
}

void NameFactory::DestroyName(Name* name) const {
	// 析构姓名
	if (!name) {
		debugf("Name is null before destroying.\n");
		return;
	}
	auto it = registries.find(name->GetType());
	if (it != registries.end()) {
		it->second.second(name);
	}
	else {
		debugf(("Deleter not found for " + name->GetType() + ".\n").data());
	}
}

