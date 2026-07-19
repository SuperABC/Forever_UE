#pragma once

#include "society/calendar_mod.h"


// 标准双休日程
class StandardCalendar : public CalendarMod {
public:
	/*
	* 构造标准双休日程
	*/
	StandardCalendar();

	/*
	* 析构标准双休日程
	*/
	virtual ~StandardCalendar();

	/*
	* Override
	* 日程静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 日程动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 日程实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 获取指定日期的上班时间
	* @date: 目标日期
	* @post: 向Core发起查询的句柄
	*/
	virtual Time SigninTime(const Time& date, PostHandle* post) const override;

	/*
	* Override
	* 获取指定日期的下班时间
	* @date: 目标日期
	* @post: 向Core发起查询的句柄
	*/
	virtual Time SignoutTime(const Time& date, PostHandle* post) const override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

	// 上班时间
	Time on;

	// 下班时间
	Time off;
};

// 全勤无休日程
class FullCalendar : public CalendarMod {
public:
	/*
	* 构造全勤无休日程
	*/
	FullCalendar();

	/*
	* 析构全勤无休日程
	*/
	virtual ~FullCalendar();

	/*
	* Override
	* 日程静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 日程动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* Override
	* 日程实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* Override
	* 获取指定日期的上班时间
	* @date: 目标日期
	* @post: 向Core发起查询的句柄
	*/
	virtual Time SigninTime(const Time& date, PostHandle* post) const override;

	/*
	* Override
	* 获取指定日期的下班时间
	* @date: 目标日期
	* @post: 向Core发起查询的句柄
	*/
	virtual Time SignoutTime(const Time& date, PostHandle* post) const override;

private:
	// 总实例数量
	static int count;

	// 实例编号
	int id;

	// 实例名称
	std::string name;

	// 上班时间
	Time on;

	// 下班时间
	Time off;
};

