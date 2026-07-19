#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/handle.h"

#include <string>
#include <functional>


// 日程Mod基类
class CalendarMod {
public:
	/*
	* 构造日程
	*/
	CalendarMod();

	/*
	* 析构日程
	*/
	virtual ~CalendarMod();

	/*
	* Override
	* 日程静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 日程动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 日程实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 获取指定日期的上班时间
	* @date: 目标日期
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME virtual Time SigninTime(const Time& date, PostHandle* post) const = 0;

	/*
	* Override
	* 获取指定日期的下班时间
	* @date: 目标日期
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME virtual Time SignoutTime(const Time& date, PostHandle* post) const = 0;

};

class CalendarFactory {
public:
	/*
	* 注册日程
	* @id: 日程类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterCalendar(const std::string& id,
		std::function<CalendarMod* ()> creator, std::function<void(CalendarMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建日程
	* @id: 日程类型
	*/
	CalendarMod* CreateCalendar(const std::string& id) const;

	/*
	* 检查日程是否已注册
	* @id: 日程类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置日程启用配置
	* @name: 日程类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 析构日程
	* @calendarMod: 待析构的日程
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

