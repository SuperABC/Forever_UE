#include "calendar_basic.h"


using namespace std;

int StandardCalendar::count = 0;

StandardCalendar::StandardCalendar() : id(count++) {

}

StandardCalendar::~StandardCalendar() {

}

const char* StandardCalendar::GetId() {
	return "standard";
}

const char* StandardCalendar::GetType() const {
	return "standard";
}

const char* StandardCalendar::GetName() {
	name = "标准双休日程" + to_string(id);
	return name.data();
}

Time StandardCalendar::SigninTime(const Time& date) const {
	int day = date.DayOfWeek();
	if (day >= 0 && day <= 4) {
		Time t = on;
		t.SetDate(date.GetYear(), date.GetMonth(), date.GetDay());
		return t;
	}
	else {
		return Time();
	}
}

Time StandardCalendar::SignoutTime(const Time& date) const {
	int day = date.DayOfWeek();
	if (day >= 0 && day <= 4) {
		Time t = off;
		t.SetDate(date.GetYear(), date.GetMonth(), date.GetDay());
		return t;
	}
	else {
		return Time();
	}
}

int FullCalendar::count = 0;

FullCalendar::FullCalendar() : id(count++) {

}

FullCalendar::~FullCalendar() {

}

const char* FullCalendar::GetId() {
	return "full";
}

const char* FullCalendar::GetType() const {
	return "full";
}

const char* FullCalendar::GetName() {
	name = "全勤无休日程" + to_string(id);
	return name.data();
}

Time FullCalendar::SigninTime(const Time& date) const {
	Time t = on;
	t.SetDate(date.GetYear(), date.GetMonth(), date.GetDay());
	return t;
}

Time FullCalendar::SignoutTime(const Time& date) const {
	Time t = off;
	t.SetDate(date.GetYear(), date.GetMonth(), date.GetDay());
	return t;
}

