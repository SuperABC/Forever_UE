#include "../common/error.h"
#include "../common/utility.h"

#include "name_base.h"


using namespace std;

// 全成员默认构造
Name::Name() :
	reserve(),
	roll() {

}

// 无析构
Name::~Name() {

}

// 占用名字
bool Name::ReserveName(const string& name) {
	if (reserve.find(name) == reserve.end()) {
		reserve.insert(name);
		return true;
	}
	return false;
}

// 使用名字
bool Name::RegisterName(const string& name) {
	if (roll.find(name) == roll.end() && reserve.find(name) == reserve.end()) {
		roll.insert(name);
		return true;
	}
	return false;
}

// 注册姓名
void NameFactory::RegisterName(const string& id,
    function<Name* ()> creator, function<void(Name*)> deleter) {
    registries[id] = { creator, deleter };
}

// 创建姓名
Name* NameFactory::CreateName(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Name %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Name %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Name " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool NameFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void NameFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 获取启用姓名
Name* NameFactory::GetName() const {
    for (const auto& config : configs) {
        if (!config.second) continue;

        auto it = registries.find(config.first);
        if (it == registries.end()) continue;

        if (!it->second.first) {
            THROW_EXCEPTION(NullPointerException, "Name " + config.first + " creator is null.\n");
        }
        return it->second.first();
    }
    return nullptr;
}

// 析构姓名
void NameFactory::DestroyName(Name* name) const {
    if (!name) {
        debugf("Warning: Name is null when deleting.\n");
        return;
    }

    auto it = registries.find(name->GetType());
    if (it == registries.end()) {
        debugf("Warning: Name %s not registered when deleting.\n", name->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(name);
    } else {
        THROW_EXCEPTION(NullPointerException, "Name " + name->GetType() + " deleter is null.\n");
    }
}

