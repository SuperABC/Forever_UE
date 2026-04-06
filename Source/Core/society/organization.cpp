#include "organization.h"

#include "map/component.h"
#include "map/room.h"
#include "society/job.h"


using namespace std;

Organization::Organization(OrganizationFactory* factory, const string& organization) :
	mod(factory->CreateOrganization(organization)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()),
	jobs() {

}

Organization::~Organization() {
	factory->DestroyOrganization(mod);
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

	if (candidates.size() == 0)return positions;

	int i = 0;
	for (auto& [component, vacancies] : jobs) {
		for (auto& [job, employee] : vacancies) {
			if (employee == nullptr) {
				positions.push_back(job);
				employee = candidates[i];
				++i;
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
		for (auto& [job, person] : vacancies) {
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


