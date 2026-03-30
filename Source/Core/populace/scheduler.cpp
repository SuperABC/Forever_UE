#include "scheduler.h"


using namespace std;

Scheduler::Scheduler(SchedulerFactory* factory, const string& component) :
	mod(factory->CreateScheduler(component)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	script(nullptr) {

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

void Scheduler::InitScheduler() {
	mod->InitScheduler();
}

std::pair<std::string, std::vector<std::string>> Scheduler::GetScriptSetup() {
	return mod->script;
}

Script* Scheduler::GetScript() const {
	return script;
}

void Scheduler::SetScript(Script* script) {
	this->script = script;
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


