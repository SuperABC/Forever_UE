#pragma once

#include "../common/condition.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>


class Change {
public:
    Change();
    virtual ~Change();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 基变更列表
    virtual std::vector<Change*> ApplyChange() = 0;

    // 父类实现方法

    // 获取控制条件
    Condition& GetCondition();

    // 设置控制条件
    void SetCondition(const Condition& condition);
    
private:
    Condition condition;
};

class ChangeFactory {
public:
    // 注册变更
    void RegisterChange(const std::string& id,
        std::function<Change*()> creator, std::function<void(Change*)> deleter);

    // 创建变更（包含new操作）
    Change* CreateChange(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构变更（包含delete操作）
    void DestroyChange(Change* change) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Change*()>, std::function<void(Change*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

