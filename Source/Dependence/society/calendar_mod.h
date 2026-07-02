#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>


class CalendarMod {
public:
	/*
	* 无构造
	*/
	CalendarMod();

	/*
	* 无析构
	*/
	virtual ~CalendarMod();

	/*
	* Override
	* 日程静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 日程动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 日程实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 获取指定日期的上班时间
	* @date: 目标日期
	*/
	COSTOM_RUNTIME virtual Time SigninTime(const Time& date) const = 0;

	/*
	* Override
	* 获取指定日期的下班时间
	* @date: 目标日期
	*/
	COSTOM_RUNTIME virtual Time SignoutTime(const Time& date) const = 0;

};

class CalendarFactory {
public:
	/*
	* 注册日程
	* @id: 日程静态类型标识
	* @creator, deleter: 构造与析构方法
	*/
	void RegisterCalendar(const std::string& id,
		std::function<CalendarMod* ()> creator, std::function<void(CalendarMod*)> deleter);

	/*
	* 清空注册
	*/
	void RemoveAll();

	/*
	* 创建日程
	* @id: 日程静态类型标识
	*/
	CalendarMod* CreateCalendar(const std::string& id) const;

	/*
	* 检查是否注册
	* @id: 日程静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 日程静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构日程
	* @calendarMod: 待析构的日程模组对象
	*/
	void DestroyCalendar(CalendarMod* calendarMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<CalendarMod* ()>, std::function<void(CalendarMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};

