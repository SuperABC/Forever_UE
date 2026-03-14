#pragma once

#include "../common/condition.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

#undef CreateEvent


class Event {
public:
    Event();
    virtual ~Event();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 判断匹配
    virtual bool operator==(Event* e) = 0;

    // 父类实现方法

    // 获取控制条件
    Condition& GetCondition();

    // 设置控制条件
    void SetCondition(const Condition& condition);
    
private:
    Condition condition;
};

class EventFactory {
public:
    // 注册事件
    void RegisterEvent(const std::string& id,
        std::function<Event*()> creator, std::function<void(Event*)> deleter);

    // 创建事件（包含new操作）
    Event* CreateEvent(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构事件（包含delete操作）
    void DestroyEvent(Event* event) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Event*()>, std::function<void(Event*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

