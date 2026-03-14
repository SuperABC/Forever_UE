#include "../common/error.h"
#include "../common/utility.h"

#include "change_base.h"


using namespace std;

// 全成员默认构造
Change::Change() :
    condition() {

}

// 无析构
Change::~Change() {

}

// 获取控制条件
Condition& Change::GetCondition() {
    return condition;
}

// 设置控制条件
void Change::SetCondition(const Condition& condition) {
    this->condition = condition;
}

// 注册变更
void ChangeFactory::RegisterChange(const string& id,
    function<Change*()> creator, function<void(Change*)> deleter) {
    registries[id] = {creator, deleter};
}

// 创建变更
Change* ChangeFactory::CreateChange(const string& id) const {
    auto config = configs.find(id);
    if (config == configs.end() || !config->second) {
        debugf("Warning: Change %s not enabled when creating.\n", id.data());
        return nullptr;
    }

    auto it = registries.find(id);
    if (it == registries.end()) {
        debugf("Warning: Change %s not registered when creating.\n", id.data());
        return nullptr;
    }

    if (it->second.first) {
        return it->second.first();
    } else {
        THROW_EXCEPTION(NullPointerException, "Change " + id + " creator is null.\n");
    }

    return nullptr;
}

// 检查是否注册
bool ChangeFactory::CheckRegistered(const string& id) const {
    return registries.find(id) != registries.end();
}

// 设置启用配置
void ChangeFactory::SetConfig(const string& name, bool config) {
    configs[name] = config;
}

// 析构变更
void ChangeFactory::DestroyChange(Change* change) const {
    if (!change) {
        debugf("Warning: Change is null when deleting.\n");
        return;
    }

    auto it = registries.find(change->GetType());
    if (it == registries.end()) {
        debugf("Warning: Change %s not registered when deleting.\n", change->GetType().data());
        return;
    }

    if (it->second.second) {
        it->second.second(change);
    } else {
        THROW_EXCEPTION(NullPointerException, "Change " + change->GetType() + " deleter is null.\n");
    }
}

