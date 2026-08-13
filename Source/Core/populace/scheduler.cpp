#include "scheduler.h"

#include "story/story.h"
#include "story/script.h"
#include "populace/person.h"


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

void Scheduler::InitScheduler(string name, const Personality& personality) {
	mod->InitScheduler();

	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", name);
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_APPEARANCE), static_cast<double>(personality[PERSONALITY_APPEARANCE]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_FITNESS), static_cast<double>(personality[PERSONALITY_FITNESS]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_ENERGY), static_cast<double>(personality[PERSONALITY_ENERGY]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_INTELLIGENCE), static_cast<double>(personality[PERSONALITY_INTELLIGENCE]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_ELOQUENCE), static_cast<double>(personality[PERSONALITY_ELOQUENCE]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_CONFIDENCE), static_cast<double>(personality[PERSONALITY_CONFIDENCE]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_MORALITY), static_cast<double>(personality[PERSONALITY_MORALITY]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_MENTALITY), static_cast<double>(personality[PERSONALITY_MENTALITY]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_IMAGINATION), static_cast<double>(personality[PERSONALITY_IMAGINATION]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_KNOWLEDGE), static_cast<double>(personality[PERSONALITY_KNOWLEDGE]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_ART), static_cast<double>(personality[PERSONALITY_ART]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_REASONING), static_cast<double>(personality[PERSONALITY_REASONING]));
	script->SetValue("self." + Personality::GetFieldName(PERSONALITY_PERCEPTION), static_cast<double>(personality[PERSONALITY_PERCEPTION]));
}

Script* Scheduler::GetScript() const {
	return script;
}

void Scheduler::DailyPlan(const Time& time, PostHandle* post) {
	mod->DailyPlan(time, post);
}

vector<Change*> Scheduler::ExecNode(const string& node, Script* storyScript,
	const vector<Script*>& jobScripts, PostHandle* post) {
	mod->changes.clear();
	vector<Container*> containerJobScripts(jobScripts.begin(), jobScripts.end());
	mod->ExecNode(node, storyScript, script, containerJobScripts, post);
	vector<Change*> result;
	for (auto& cv : mod->changes) {
		result.push_back(visit([](auto& c) -> Change* { return new decay_t<decltype(c)>(c); }, cv));
	}
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

void EmptyScheduler::DailyPlan(const Time& time, PostHandle* post) {

}

void EmptyScheduler::ExecNode(const string& node,
	Container* storyScript, Container* schedulerScript, const vector<Container*>& jobScripts,
	PostHandle* post) {

}

