#pragma once

#include "../common/utility.h"

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>


class Calendar {
public:
	Calendar();
	virtual ~Calendar();

	// 子类实现方法

	// 统一类型定义
	static std::string GetId();
	virtual std::string GetType() const = 0;
	virtual std::string GetName() const = 0;

	// 指定上班时间
	virtual Time SigninTime(const Time& date) const = 0;

	// 指定下班时间
	virtual Time SignoutTime(const Time& date) const = 0;

	// 父类实现方法

private:
};

class CalendarFactory {
public:
    // 注册日程
    void RegisterCalendar(const std::string& id,
        std::function<Calendar* ()> creator, std::function<void(Calendar*)> deleter);

    // 创建日程（包含new操作）
    Calendar* CreateCalendar(const std::string& id) const;

    // 检查是否注册
    bool CheckRegistered(const std::string& id) const;

    // 设置启用配置
    void SetConfig(const std::string& name, bool config);

    // 析构日程（包含delete操作）
    void DestroyCalendar(Calendar* calendar) const;

private:
    std::unordered_map<
        std::string,
        std::pair<std::function<Calendar* ()>, std::function<void(Calendar*)>>
    > registries;
    std::unordered_map<std::string, bool> configs;
};

