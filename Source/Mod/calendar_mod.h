#pragma once

#include "../common/error.h"
#include "../common/utility.h"

#include "calendar_base.h"

#include <memory>
#include <string>


// 模组检测子类
class ModCalendar : public Calendar {
public:
	ModCalendar() {}
	virtual ~ModCalendar() {}

	static std::string GetId() { return "mod"; }
	virtual std::string GetType() const override { return "mod"; }
	virtual std::string GetName() const override { return "模组日程"; }

	virtual Time SigninTime(const Time& date) const override {
		Time t("09:00:00.000");
		t.SetDate(date.GetYear(), date.GetMonth(), date.GetDay());
		return t;
	}
	virtual Time SignoutTime(const Time& date) const override {
		Time t("17:00:00.000");
		t.SetDate(date.GetYear(), date.GetMonth(), date.GetDay());
		return t;
	}
};

