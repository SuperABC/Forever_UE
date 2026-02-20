#pragma once

#include "../common/utility.h"
#include "../map/room_base.h"
#include "../society/calendar_base.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>


class Job {
public:
    Job() = default;
    virtual ~Job() = default;

	// 子类实现方法

    // 动态返回职业静态信息
    static std::string GetId();
    virtual std::string GetType() const = 0;
    virtual std::string GetName() const = 0;

    // 职业剧本
    virtual std::vector<std::string> GetScripts() const = 0;

	// 父类实现方法

	// 设置/获取日历
    void SetCalendar(Calendar* calendar);
    Calendar* GetCalendar() const;

    // 设置/获取工位
    void SetPosition(Room* room);
    Room* GetPosition() const;

protected:
    Calendar* calendar;

    Room* position;
};

class JobFactory {
public:
    void RegisterJob(const std::string& id, std::function<Job*()> creator);
    Job* CreateJob(const std::string& id);
    bool CheckRegistered(const std::string& id);
    void SetConfig(std::string name, bool config);

private:
    std::unordered_map<std::string, std::function<Job* ()>> registries;
    std::unordered_map<std::string, bool> configs;
};