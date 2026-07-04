#include "organization.h"

#include "map/component.h"
#include "map/room.h"
#include "populace/person.h"
#include "society/job.h"
#include "story/story.h"
#include "story/script.h"


using namespace std;

Organization::Organization(OrganizationFactory* factory, const string& organization) :
	mod(factory->CreateOrganization(organization)),
	factory(factory),
	type(),
	name(),
	count(0),
	jobs(),
	script(nullptr) {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Organization " + organization + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Organization::~Organization() {
	factory->DestroyOrganization(mod);
	for (auto& [_, vacancies] : jobs) {
		for (auto& [job, _] : vacancies) {
			delete job;
		}
	}
	if (script) delete script;
	script = nullptr;
}

string Organization::GetType() const {
	return type;
}

string Organization::GetName() const {
	return name;
}

unordered_map<string, pair<int, int>> Organization::ComponentRequirements() const {
	mod->ComponentRequirements();
	return mod->requirements;
}

unordered_map<string, vector<vector<pair<string, string>>>> Organization::ArrageVacancies(
	const unordered_map<string, int>& components) {
	mod->ArrageVacancies(components);
	return mod->vacancies;
}

void Organization::AddVacancy(Component* component, const vector<Job*>& vacancies) {
	if (component == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Vacancy component is null.\n");
	}
	vector<pair<Job*, Person*>> positions;
	for (auto vacancy : vacancies) {
		positions.emplace_back(vacancy, nullptr);
	}
	jobs.emplace_back(component, positions);
}

vector<Job*> Organization::EnrollEmployee(const vector<Person*>& candidates) {
	vector<Job*> positions;

	if (candidates.size() == 0) return positions;

	int i = 0;
	for (auto& [component, vacancies] : jobs) {
		for (auto& [job, employee] : vacancies) {
			if (!employee) {
				positions.push_back(job);
				employee = candidates[i];
				script->SetValue("self.employees[" + to_string(count) + "].name", employee->GetName());
				script->SetValue("self.employees[" + to_string(count) + "].job", job->GetType());
				script->SetValue("self.employees[" + to_string(count) + "].position", job->GetPosition()->GetAddress());
				script->SetValue("self.employees[" + to_string(count) + "].component", component->GetName());
				script->SetValue("self.employee_count", ++count);
				i++;
				if (i >= candidates.size()) {
					return positions;
				}
			}
		}
	}

	return positions;
}

void Organization::ArrangeRooms() {
	for (auto& [component, vacancies] : jobs) {
		vector<string> rooms;
		for (auto room : component->GetRooms()) {
			rooms.push_back(room->GetType());
		}
		vector<pair<string, int>> arrages;
		for (auto& [job, _] : vacancies) {
			arrages.emplace_back(job->GetType(), -1);
		}
		mod->ArrageRoom(arrages, rooms);
		for (int i = 0; i < arrages.size(); i++) {
			if (arrages[i].second >= 0) {
				vacancies[i].first->SetPosition(component->GetRooms()[arrages[i].second]);
			}
		}
	}
}

vector<pair<Component*, vector<pair<Job*, Person*>>>> Organization::GetJobs() const {
	return jobs;
}

Script* Organization::GetScript() const {
	return script;
}

void Organization::InitOrganization() {
	mod->InitOrganization();
	script = new Script(Story::scriptFactory, mod->script.first);
	for (auto s : mod->script.second) {
		script->ReadMilestones(Config::GetScript(s));
	}
	script->SetValue("self.name", string(mod->GetName()));
	script->SetValue("self.type", string(mod->GetType()));
}

void Organization::DailyPlan(const Time& time) {
	mod->DailyPlan(time);
}

vector<Change*> Organization::ExecNode(const string& node, Script* storyScript) {
	mod->changes.clear();
	mod->ExecNode(node, storyScript, script);
	vector<Change*> result;
	for (auto& cv : mod->changes) {
		result.push_back(visit([](auto& c) -> Change* { return new decay_t<decltype(c)>(c); }, cv));
	}
	mod->changes.clear();
	return result;
}

const unordered_map<string, Time>& Organization::GetPlans() const {
	return mod->plans;
}

int EmptyOrganization::count = 0;

EmptyOrganization::EmptyOrganization() : id(count++) {

}

EmptyOrganization::~EmptyOrganization() {

}

const char* EmptyOrganization::GetId() {
	return "empty";
}

const char* EmptyOrganization::GetType() const {
	return "empty";
}

const char* EmptyOrganization::GetName() {
	name = "空组织" + to_string(id);
	return name.data();
}

float EmptyOrganization::GetPower() {
	return 0.f;
}

void EmptyOrganization::ComponentRequirements() {

}

void EmptyOrganization::ArrageVacancies(const unordered_map<string, int>& components) {

}

void EmptyOrganization::ArrageRoom(vector<pair<string, int>>& arrages,
	const vector<string>& rooms) {

}

void EmptyOrganization::InitOrganization() {
	script = { "empty", {} };
}

void EmptyOrganization::DailyPlan(const Time& time) {

}

void EmptyOrganization::ExecNode(const string& name, Container* storyScript, Container* orgScript) {

}


