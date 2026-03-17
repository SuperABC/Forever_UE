#pragma once

#include "../map/room_base.h"
#include "../society/calendar_base.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>


class Job {
public:
    Job();
    virtual ~Job();

    // 子类实现方法

    // 统一类型定义
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 职业剧本
    virtual std::vector<std::string> GetScripts() const = 0;

    // 父类实现方法

    // 获取工作日历
    Calendar* GetCalendar() const;

    // 设置工作日历
    void SetCalendar(Calendar* calendar);

    // 获取工作房间
    Room* GetPosition() const;

    // 设置工作房间
    void SetPosition(Room* room);

private:
    // 工作日历
    Calendar* calendar;

    // 工作房间
    Room* position;
};

class JobFactory {
public:
    // 清空注册
    void RemoveAll();

    // 注册工作
    void RegisterJob(const std::string& id,
        std::function<Job* ()> creator, std::function<void(Job*)> deleter);

    // 创建工作（包含new操作）
    Job* CreateJob(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构工作（包含delete操作）
    void DestroyJob(Job* job) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Job* ()>, std::function<void(Job*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

