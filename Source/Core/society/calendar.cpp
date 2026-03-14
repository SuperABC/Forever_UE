#include "calendar.h"


using namespace std;

StandardCalendar::StandardCalendar() :
	on(Time(string("9:00:00.000"))), off(Time(string("17:00:00.000"))) {

}

StandardCalendar::~StandardCalendar() {

}

string StandardCalendar::GetId() {
	return "standard";
}

string StandardCalendar::GetType() const {
	return "standard";
}

string StandardCalendar::GetName() const {
	return "标准双休日程";
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

FullCalendar::FullCalendar() :
	on(Time(string("00:00:01.000"))), off(Time(string("23:59:59.000"))) {

}

FullCalendar::~FullCalendar() {

}

string FullCalendar::GetId() {
	return "full";
}

string FullCalendar::GetType() const {
	return "full";
}

string FullCalendar::GetName() const {
	return "全勤无休日程";
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

