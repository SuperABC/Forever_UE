#pragma once

#include "society/calendar_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModCalendarsFunc)(CalendarFactory* factory);

// 日程实体
class Calendar {
public:
	// 禁止默认构造
	Calendar() = delete;

	// 通过类型从工厂构造
	Calendar(CalendarFactory* factory, const std::string& calendar);

	// 析构日程
	~Calendar();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 上班时间
	Time SigninTime(const Time& date) const;

	// 下班时间
	Time SignoutTime(const Time& date) const;

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
	EmptyCalendar();
	virtual ~EmptyCalendar();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual Time SigninTime(const Time& date) const;
	virtual Time SignoutTime(const Time& date) const;

private:
	static int count;

	int id;
	std::string name;
};

