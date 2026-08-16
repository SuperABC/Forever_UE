#pragma once

#include "society/calendar_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModCalendarsFunc)(CalendarFactory* factory);

// 子类注册收尾函数
typedef void (*FinishModCalendarsFunc)(CalendarFactory* factory);

// 日程实体
class Calendar {
public:
	/*
	* 禁止默认构造
	*/
	Calendar() = delete;

	/*
	* 通过类型从工厂构造
	* @factory: 日程工厂
	* @calendar: 日程静态类型标识
	*/
	Calendar(CalendarFactory* factory, const std::string& calendar);

	/*
	* 析构日程
	*/
	~Calendar();

	/*
	* 获取类型
	*/
	std::string GetType() const;

	/*
	* 获取名称
	*/
	std::string GetName() const;

	/*
	* 获取指定日期的上班时间
	* @date: 目标日期
	* @post: 向Core发起查询的句柄
	*/
	Time SigninTime(const Time& date, PostHandle* post) const;

	/*
	* 获取指定日期的下班时间
	* @date: 目标日期
	* @post: 向Core发起查询的句柄
	*/
	Time SignoutTime(const Time& date, PostHandle* post) const;

private:
	// 模组对象
	OBJECT_HOLDER CalendarMod* mod;

	// 工厂
	CalendarFactory* factory;

	// 日程类型
	std::string type;

	// 日程名称
	std::string name;

};

// 空日程
class EmptyCalendar : public CalendarMod {
public:
	/*
	* 构造空日程
	*/
	EmptyCalendar();

	/*
	* 析构空日程
	*/
	virtual ~EmptyCalendar();

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
};

