#include "person.h"


using namespace std;

Person::Person() {

}

Person::~Person() {
	for(auto asset : assets) {
		delete asset;
	}
	for(auto job : jobs) {
		delete job;
	}
	delete scheduler;
	for(auto script : scripts) {
		delete script;
	}
}

int Person::GetId() const {
	return id;
}

void Person::SetId(int id) {
	this->id = id;
}

const string& Person::GetName() const {
	return name;
}

void Person::SetName(const string& name) {
	this->name = name;
}

GENDER_TYPE Person::GetGender() const {
	return gender;
}

void Person::SetGender(GENDER_TYPE gender) {
	this->gender = gender;
}

float Person::GetHeight() const {
	return height;
}

void Person::SetHeight(float height) {
	this->height = height;
}

float Person::GetWeight() const {
	return weight;
}

void Person::SetWeight(float weight) {
	this->weight = weight;
}

const Time& Person::GetBirthday() const {
	return birthday;
}

void Person::SetBirthday(const Time& birthday) {
	this->birthday = birthday;
}

int Person::GetAge(const Time* currentTime) const {
	int age = currentTime->GetYear() - birthday.GetYear();
	if (currentTime->GetMonth() < birthday.GetMonth() ||
		(currentTime->GetMonth() == birthday.GetMonth() && currentTime->GetDay() < birthday.GetDay())) {
		age--;
	}
	return age;
}

const Time& Person::GetMarryday() const {
	return marryday;
}

void Person::SetMarryday(const Time& marryday) {
	this->marryday = marryday;
}

const string& Person::GetNick() const {
	return nick;
}

void Person::SetNick(const string& nick) {
	this->nick = nick;
}

int Person::GetDeposit() const {
	return deposit;
}

void Person::SetDeposit(int deposit) {
	this->deposit = deposit;
}

int Person::GetPhone() const {
	return phone;
}

void Person::SetPhone(int phone) {
	this->phone = phone;
}

int Person::GetSimulate() const {
	return simulate;
}

void Person::SetSimulate(int simulate) {
	this->simulate = simulate;
}

void Person::AddRelative(RELATIVE_TYPE type, Person* person) {
	relatives.push_back(make_pair(type, person));
}

Person* Person::GetFather() {
	for (auto relative : relatives) {
		if (relative.first == RELATIVE_FATHER)return relative.second;
	}
	return nullptr;
}

Person* Person::GetMother() {
	for (auto relative : relatives) {
		if (relative.first == RELATIVE_MOTHER)return relative.second;
	}
	return nullptr;
}

Person* Person::GetSpouse() {
	for (auto relative : relatives) {
		if (relative.first == RELATIVE_WIFE || relative.first == RELATIVE_HUSBAND)return relative.second;
	}

	return nullptr;
}

vector<Person*> Person::GetChilds() {
	vector<Person*>childs;
	for (auto relative : relatives) {
		if (relative.first == RELATIVE_SON || relative.first == RELATIVE_DAUGHTER)childs.push_back(relative.second);
	}
	return childs;
}

void Person::AddAsset(Asset* asset) {
	assets.push_back(asset);
}

vector<Asset*>& Person::GetAssets() {
	return assets;
}

vector<Asset*> Person::GetAssets(string type) {
	vector<Asset*> results;

	for (auto& asset : assets) {
		if (asset->GetType() == type) {
			results.push_back(asset);
		}
	}

	return results;
}

Asset* Person::GetAsset(string name) {
	for (auto& asset : assets) {
		if (asset->GetName() == name) {
			return asset;
		}
	}

	return nullptr;
}

vector<Job*> Person::GetJobs() {
	return jobs;
}

void Person::AddJob(Job* job) {
	jobs.push_back(job);
}

void Person::RemoveJob(Job* job) {
	for (auto &j : jobs) {
		if (j == job) {
			j = *jobs.end();
			jobs.pop_back();
			break;
		}
	}
}

void Person::SetWork(int job) {
	working = job;
}

Job* Person::GetWork() {
	return jobs[working];
}

Room* Person::GetHome() {
	return home;
}

void Person::SetHome(Room* room) {
	home = room;
}

void Person::RemoveHome() {
	home = nullptr;
}

Scheduler* Person::GetScheduler() {
	return scheduler;
}

void Person::SetScheduler(Scheduler* scheduler) {
	this->scheduler = scheduler;
}

void Person::AddEducationExperience(EducationExperience exp) {
	educationExperiences.push_back(exp);
}

void Person::AddEmotionExperience(EmotionExperience exp) {
	emotionExperiences.push_back(exp);
}

void Person::AddJobExperience(JobExperience exp) {
	jobExperiences.push_back(exp);
}

vector<EducationExperience>& Person::GetEducationExperiences() {
	return educationExperiences;
}

vector<EmotionExperience>& Person::GetEmotionExperiences() {
	return emotionExperiences;
}

vector<JobExperience>& Person::GetJobExperiences() {
	return jobExperiences;
}

void Person::ExperienceComposition() {
	if (educationExperiences.size() == 0)return;

	int idx = 0;
	EducationExperience education = educationExperiences[0];
	for (int i = 1; i < educationExperiences.size(); i++) {
		if (education.GetSchool() == educationExperiences[i].GetSchool() &&
			education.GetEndTime().GetYear() == educationExperiences[i].GetBeginTime().GetYear()) {
			education.SetTime(education.GetBeginTime(), educationExperiences[i].GetEndTime());
			education.SetGraduate(educationExperiences[i].GetGraduate());
		}
		else {
			educationExperiences[idx++] = education;
			education = educationExperiences[i];
		}
	}
	educationExperiences[idx++] = education;
	educationExperiences.resize(idx);
}

void Person::AddScript(Script* script) {
	scripts.push_back(script);
}

pair<vector<Dialog>, vector<Change*>> Person::MatchEvent(
	Event* event, Story* story, Person* person) {
	pair<vector<Dialog>, vector<Change*>> results;
	for (auto& script : scripts) {
		auto result = script->MatchEvent(event, story, person);
		results.first.insert(results.first.end(), result.first.begin(), result.first.end());
		results.second.insert(results.second.end(), result.second.begin(), result.second.end());
	}
	return results;
}

pair<bool, ValueType> Person::GetValue(const string& name) {
	auto it = variables.find(name);
	if (it != variables.end()) {
		return { true, it->second };
	}
	return { false, 0 };
}

void Person::UpdateValues(Time *t) {
	variables["self.name"] = name;
	variables["self.gender"] = gender == GENDER_FEMALE ? "female" : "male";
	variables["self.age"] = GetAge(t);
}

void Person::SetValue(const string& name, ValueType value) {
	variables[name] = value;
}

bool Person::AddOption(string option) {
	if (options.find(option) == options.end()) {
		options.insert(option);
		return true;
	}
	return false;
}

bool Person::RemoveOption(string option) {
	if (options.find(option) != options.end()) {
		options.erase(option);
		return true;
	}
	return false;
}

unordered_set<string> Person::GetOptions() {
	return options;
}

//void Person::SetStatus(shared_ptr<Zone> zone) {
//	currentPlot = zone->GetParent();
//	currentZone = zone;
//	currentBuilding = nullptr;
//	currentRoom = nullptr;
//	commute.SetNull();
//}
//
//void Person::SetStatus(shared_ptr<Building> building) {
//	currentPlot = building->GetParentPlot();
//	currentZone = building->GetParentZone();
//	currentBuilding = building;
//	currentRoom = nullptr;
//	commute.SetNull();
//}
//
//void Person::SetStatus(shared_ptr<Room> room) {
//	currentPlot = room->GetParentBuilding()->GetParentPlot();
//	currentZone = room->GetParentBuilding()->GetParentZone();
//	currentBuilding = room->GetParentBuilding();
//	currentRoom = room;
//	commute.SetNull();
//}
//
//void Person::SetStatus(shared_ptr<Room> target, vector<Connection> paths, Time time) {
//	currentPlot = nullptr;
//	currentZone = nullptr;
//	currentBuilding = nullptr;
//	currentRoom = nullptr;
//	commute.SetTarget(target);
//	commute.SetPaths(paths);
//	commute.StartCommute(time);
//}
//
//shared_ptr<Plot> Person::GetCurrentPlot() {
//	return currentPlot;
//}
//
//shared_ptr<Zone> Person::GetCurrentZone() {
//	return currentZone;
//}
//
//shared_ptr<Building> Person::GetCurrentBuilding() {
//	return currentBuilding;
//}
//
//shared_ptr<Room> Person::GetCurrentRoom() {
//	return currentRoom;
//}
//
//Commute Person::GetCurrentCommute() {
//	return commute;
//}

