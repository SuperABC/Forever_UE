#include "job.h"

#include "society/society.h"
#include "society/calendar.h"
#include "story/story.h"
#include "story/script.h"


using namespace std;

Job::Job(JobFactory* factory, const string& job) :
	mod(factory->CreateJob(job)),
	factory(factory),
	type(),
	name(),
	calendar(nullptr),
	script(nullptr),
	position(nullptr) {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Job " + job + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Job::~Job() {
	factory->DestroyJob(mod);

	if (calendar) delete calendar;
	calendar = nullptr;
	if (script) delete script;
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

void Job::SetCalendar(Calendar* calendar) {
	this->calendar = calendar;
}

void Job::InitJob(string name, int idx) {
	mod->InitJob();
	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", name);
	script->SetValue("self.idx", idx);
}

Script* Job::GetScript() const {
	return script;
}

Room* Job::GetPosition() const {
	return position;
}

void Job::SetPosition(Room* room) {
	position = room;
}

void Job::DailyPlan() {
	mod->plans.clear();
	mod->DailyPlan();
}

vector<Change*> Job::ExecNode(const string& node, Script* storyScript, Script* schedulerScript) {
	mod->changes.clear();
	mod->ExecNode(node, storyScript, schedulerScript, script);
	vector<Change*> result = move(mod->changes);
	mod->changes.clear();
	return result;
}

const unordered_map<string, Time>& Job::GetPlans() const {
	return mod->plans;
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

void EmptyJob::DailyPlan() {

}

void EmptyJob::ExecNode(const string& name, Script* storyScript, Script* schedulerScript, Script* jobScript) {

}


