#include "society.h"
#include "job.h"


using namespace std;

Job::Job(JobFactory* factory, const string& component) :
	mod(factory->CreateJob(component)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	calendar(nullptr),
	script(nullptr) {

}

Job::~Job() {
	factory->DestroyJob(mod);

	if (calendar)delete calendar;
	calendar = nullptr;
	if (script)delete script;
	script = nullptr;
}

string Job::GetType() const {
	return type;
}

string Job::GetName() const {
	return name;
}

Calendar* Job::GetCalendar() const {
	return calendar;
}

void Job::InitJob() {
	calendar = new Calendar(Society::calendarFactory, mod->calendar);

	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", name);
}

Script* Job::GetScript() const {
	return script;
}

int EmptyJob::count = 0;

EmptyJob::EmptyJob() : id(count++) {

}

EmptyJob::~EmptyJob() {

}

const char* EmptyJob::GetId() {
	return "empty";
}

const char* EmptyJob::GetType() const {
	return "empty";
}

const char* EmptyJob::GetName() {
	name = "空职业" + to_string(id);
	return name.data();
}

void EmptyJob::InitJob() {

}


