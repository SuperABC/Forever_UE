#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class CalendarMod {
public:
	// 无构造
	CalendarMod();

	// 无析构
	virtual ~CalendarMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 上班时间
	virtual Time SigninTime(const Time& date) const = 0;

	// 下班时间
	virtual Time SignoutTime(const Time& date) const = 0;

};

class CalendarFactory {
public:
	// 注册日程
	void RegisterCalendar(const std::string& id,
		std::function<CalendarMod* ()> creator, std::function<void(CalendarMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建日程
	CalendarMod* CreateCalendar(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 析构日程
	void DestroyCalendar(CalendarMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<CalendarMod* ()>, std::function<void(CalendarMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

