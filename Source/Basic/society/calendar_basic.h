#pragma once

#include "society/calendar_mod.h"


// 标准双休日程
class StandardCalendar : public CalendarMod {
public:
	StandardCalendar();
	virtual ~StandardCalendar();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual Time SigninTime(const Time& date) const;
	virtual Time SignoutTime(const Time& date) const;

private:
	static int count;

	int id;
	std::string name;

	Time on, off;
};

// 全勤无休日程
class FullCalendar : public CalendarMod {
public:
	FullCalendar();
	virtual ~FullCalendar();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	virtual Time SigninTime(const Time& date) const;
	virtual Time SignoutTime(const Time& date) const;

private:
	static int count;

	int id;
	std::string name;

	Time on, off;
};

