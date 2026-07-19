#include "calendar.h"


using namespace std;

Calendar::Calendar(CalendarFactory* factory, const string& calendar) :
	mod(factory->CreateCalendar(calendar)),
	factory(factory),
	type(),
	name() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Calendar " + calendar + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Calendar::~Calendar() {
	factory->DestroyCalendar(mod);
}

string Calendar::GetType() const {
	return type;
}

string Calendar::GetName() const {
	return name;
}

Time Calendar::SigninTime(const Time& date, PostHandle* post) const {
	return mod->SigninTime(date, post);
}

Time Calendar::SignoutTime(const Time& date, PostHandle* post) const {
	return mod->SignoutTime(date, post);
}

int EmptyCalendar::count = 0;

EmptyCalendar::EmptyCalendar() : id(count++) {

}

EmptyCalendar::~EmptyCalendar() {

}

const char* EmptyCalendar::GetId() {
	return "empty";
}

const char* EmptyCalendar::GetType() const {
	return "empty";
}

const char* EmptyCalendar::GetName() {
	name = "空日程" + to_string(id);
	return name.data();
}

Time EmptyCalendar::SigninTime(const Time& date, PostHandle* post) const {
	return Time();
}

Time EmptyCalendar::SignoutTime(const Time& date, PostHandle* post) const {
	return Time();
}


