#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>


class Scheduler {
public:
    Scheduler();
    virtual ~Scheduler();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 调度权重
    static float GetPower();

    // 父类实现方法

    // 获取状态
    std::string GetStatus() const;

    // 设置状态
    void SetStatus(const std::string& status);

private:
    std::string status;
};

class SchedulerFactory {
public:
    // 注册调度
    void RegisterScheduler(const std::string& id, float power,
        std::function<Scheduler* ()> creator, std::function<void(Scheduler*)> deleter);

    // 创建调度（包含new操作）
    Scheduler* CreateScheduler(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 获取全部调度权重
    const std::unordered_map<std::string, float>& GetPowers() const;

    // 析构调度（包含delete操作）
    void DestroyScheduler(Scheduler* scheduler) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Scheduler* ()>, std::function<void(Scheduler*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
    std::unordered_map<std::string, float> powers;
};

