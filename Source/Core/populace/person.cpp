#include "person.h"


using namespace std;

Person::Person() :
	id(0), 
	name(),
	gender(GENDER_FEMALE),
	birthday(),
	marryday(),
	height(0.0f),
	weight(0.0f),
	nick(),
	deposit(0),
	phone(0),
	relatives(),
	acquaintances(),
	assets(),
	jobs(),
	working(-1),
	home(nullptr),
	scheduler(nullptr),
	educationExperiences(),
	emotionExperiences(),
	jobExperiences(),
	options() {
	//currentPlot(nullptr),
	//currentZone(nullptr),
	//currentBuilding(nullptr),
	//currentRoom(nullptr),
	//commute() {

}

Person::~Person() {
	for (auto& asset : assets) {
		if (asset)delete asset;
	}
	assets.clear();

	for (auto& job : jobs) {
		if (job)delete job;
	}
	jobs.clear();

	if (scheduler)delete scheduler;
	scheduler = nullptr;
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
	if (currentTime == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Time is null.\n");
	}
	int age = currentTime->GetYear() - birthday.GetYear();
	if (currentTime->GetMonth() < birthday.GetMonth() ||
		(currentTime->GetMonth() == birthday.GetMonth() &&
			currentTime->GetDay() < birthday.GetDay())) {
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

void Person::AddRelative(RELATIVE_TYPE type, Person* person) {
	if (person == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Relative is null.\n");
	}
	relatives.emplace_back(type, person);
}

Person* Person::GetFather() const {
	for (const auto& [type, person] : relatives) {
		if (type == RELATIVE_FATHER) return person;
	}
	return nullptr;
}

Person* Person::GetMother() const {
	for (const auto& [type, person] : relatives) {
		if (type == RELATIVE_MOTHER) return person;
	}
	return nullptr;
}

Person* Person::GetSpouse() const {
	for (const auto& [type, person] : relatives) {
		if (type == RELATIVE_WIFE || type == RELATIVE_HUSBAND) return person;
	}
	return nullptr;
}

vector<Person*> Person::GetChilds() const {
	vector<Person*> childs;
	for (const auto& [type, person] : relatives) {
		if (type == RELATIVE_SON || type == RELATIVE_DAUGHTER) {
			childs.push_back(person);
		}
	}
	return childs;
}

void Person::AddAsset(Asset* asset) {
	if (asset == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Asset is null.\n");
	}
	assets.push_back(asset);
}

vector<Asset*>& Person::GetAssets() {
	return assets;
}

vector<Asset*> Person::GetAssets(const string& type) const {
	vector<Asset*> results;
	for (const auto& asset : assets) {
		if (asset->GetType() == type) {
			results.push_back(asset);
		}
	}
	return results;
}

Asset* Person::GetAsset(const string& name) const {
	for (const auto& asset : assets) {
		if (asset->GetName() == name) {
			return asset;
		}
	}
	return nullptr;
}

vector<Job*> Person::GetJobs() const {
	return jobs;
}

void Person::AddJob(Job* job) {
	if (job == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Job is null.\n");
	}
	jobs.push_back(job);
}

void Person::RemoveJob(Job* job) {
	if (job == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Job is null.\n");
	}
	for (auto it = jobs.begin(); it != jobs.end(); ++it) {
		if (*it == job) {
			auto index = it - jobs.begin();
			if (working == index) {
				working = -1;
			}
			else if (working > index) {
				--working;
			}
			delete *it;
			jobs.erase(it);
			break;
		}
	}
}

void Person::SetWork(int job) {
	if (job < 0 || job >= static_cast<int>(jobs.size())) {
		THROW_EXCEPTION(OutOfRangeException, "Job index out of range.\n");
	}
	working = job;
}

Job* Person::GetWork() const {
	if (working < 0 || working >= static_cast<int>(jobs.size())) {
		THROW_EXCEPTION(OutOfRangeException, "Working index out of range.\n");
	}
	return jobs[working];
}

Room* Person::GetHome() const {
	return home;
}

void Person::SetHome(Room* room) {
	home = room;
}

void Person::RemoveHome() {
	home = nullptr;
}

Scheduler* Person::GetScheduler() const {
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
	if (educationExperiences.empty()) {
		return;
	}

	int idx = 0;
	EducationExperience education = educationExperiences[0];
	for (size_t i = 1; i < educationExperiences.size(); ++i) {
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

bool Person::AddOption(const string& option) {
	if (options.find(option) == options.end()) {
		options.insert(option);
		return true;
	}
	return false;
}

bool Person::RemoveOption(const string& option) {
	if (options.find(option) != options.end()) {
		options.erase(option);
		return true;
	}
	return false;
}

unordered_set<string> Person::GetOptions() const {
	return options;
}

//void Person::SetStatus(Zone* zone) {
//	if (zone == nullptr) {
//		THROW_EXCEPTION(NullPointerException, "Zone is null.\n");
//	}
//	currentPlot = zone->GetParent();
//	currentZone = zone;
//	currentBuilding = nullptr;
//	currentRoom = nullptr;
//	commute.SetNull();
//}
//
//void Person::SetStatus(Building* building) {
//	if (building == nullptr) {
//		THROW_EXCEPTION(NullPointerException, "Building is null.\n");
//	}
//	currentPlot = building->GetParentPlot();
//	currentZone = building->GetParentZone();
//	currentBuilding = building;
//	currentRoom = nullptr;
//	commute.SetNull();
//}
//
//void Person::SetStatus(Room* room) {
//	if (room == nullptr) {
//		THROW_EXCEPTION(NullPointerException, "Room is null.\n");
//	}
//	currentPlot = room->GetParentBuilding()->GetParentPlot();
//	currentZone = room->GetParentBuilding()->GetParentZone();
//	currentBuilding = room->GetParentBuilding();
//	currentRoom = room;
//	commute.SetNull();
//}
//
//void Person::SetStatus(Room* target, vector<Connection> paths, Time time) {
//	if (target == nullptr) {
//		THROW_EXCEPTION(NullPointerException, "Target room is null.\n");
//	}
//	currentPlot = nullptr;
//	currentZone = nullptr;
//	currentBuilding = nullptr;
//	currentRoom = nullptr;
//	commute.SetTarget(target);
//	commute.SetPaths(paths);
//	commute.StartCommute(time);
//}
//
//Plot* Person::GetCurrentPlot() const {
//	return currentPlot;
//}
//
//Zone* Person::GetCurrentZone() const {
//	return currentZone;
//}
//
//Building* Person::GetCurrentBuilding() const {
//	return currentBuilding;
//}
//
//Room* Person::GetCurrentRoom() const {
//	return currentRoom;
//}
//
//Commute Person::GetCurrentCommute() const {
//	return commute;
//}

