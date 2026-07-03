#include "scheduler.h"

#include "story/story.h"
#include "story/script.h"


using namespace std;

Scheduler::Scheduler(SchedulerFactory* factory, const string& scheduler) :
	mod(factory->CreateScheduler(scheduler)),
	factory(factory),
	type(),
	name(),
	script(nullptr) {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Scheduler " + scheduler + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Scheduler::~Scheduler() {
	factory->DestroyScheduler(mod);

	if (script)delete script;
	script = nullptr;
}

string Scheduler::GetType() const {
	return type;
}

string Scheduler::GetName() const {
	return name;
}

void Scheduler::InitScheduler(string name) {
	mod->InitScheduler();

	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", name);
}

Script* Scheduler::GetScript() const {
	return script;
}

void Scheduler::DailyPlan() {
	mod->plans.clear();
	mod->DailyPlan();
}

vector<Change*> Scheduler::ExecNode(const string& node, Script* storyScript, const vector<Script*>& jobScripts) {
	mod->changes.clear();
	mod->ExecNode(node, storyScript, script, jobScripts);
	vector<Change*> result = move(mod->changes);
	mod->changes.clear();
	return result;
}

const unordered_map<string, Time>& Scheduler::GetPlans() const {
	return mod->plans;
}

int EmptyScheduler::count = 0;

EmptyScheduler::EmptyScheduler() : id(count++) {

}

EmptyScheduler::~EmptyScheduler() {

}

const char* EmptyScheduler::GetId() {
	return "empty";
}

const char* EmptyScheduler::GetType() const {
	return "empty";
}

const char* EmptyScheduler::GetName() {
	name = "空调度" + to_string(id);
	return name.data();
}

float EmptyScheduler::GetPower() {
	return 0.f;
}

void EmptyScheduler::InitScheduler() {

}

void EmptyScheduler::DailyPlan() {

}

void EmptyScheduler::ExecNode(const string& node,
	Script* storyScript, Script* schedulerScript, const vector<Script*>& jobScripts) {

}

