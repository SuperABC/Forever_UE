#include "person.h"

#include "map/block.h"
#include "map/zone.h"
#include "map/building.h"
#include "map/room.h"
#include "populace/scheduler.h"
#include "populace/commute.h"
#include "populace/experience.h"
#include "society/job.h"
#include "story/script.h"
#include "player/asset.h"

#include <algorithm>


using namespace std;

Personality::Personality() :
	appearance(GetRandom(1000) / 1000.f),
	fitness(GetRandom(1000) / 1000.f),
	energy(GetRandom(1000) / 1000.f),
	intelligence(GetRandom(1000) / 1000.f),
	eloquence(GetRandom(1000) / 1000.f),
	confidence(GetRandom(1000) / 1000.f),
	morality(GetRandom(1000) / 1000.f),
	mentality(GetRandom(1000) / 1000.f),
	imagination(GetRandom(1000) / 1000.f),
	knowledge(GetRandom(1000) / 1000.f),
	art(GetRandom(1000) / 1000.f),
	reasoning(GetRandom(1000) / 1000.f),
	perception(GetRandom(1000) / 1000.f) {

}

float& Personality::operator[](PERSONALITY_TYPE type) {
	switch (type) {
	case PERSONALITY_APPEARANCE: return appearance;
	case PERSONALITY_FITNESS: return fitness;
	case PERSONALITY_ENERGY: return energy;
	case PERSONALITY_INTELLIGENCE: return intelligence;
	case PERSONALITY_ELOQUENCE: return eloquence;
	case PERSONALITY_CONFIDENCE: return confidence;
	case PERSONALITY_MORALITY: return morality;
	case PERSONALITY_MENTALITY: return mentality;
	case PERSONALITY_IMAGINATION: return imagination;
	case PERSONALITY_KNOWLEDGE: return knowledge;
	case PERSONALITY_ART: return art;
	case PERSONALITY_REASONING: return reasoning;
	case PERSONALITY_PERCEPTION: return perception;
	default:
		THROW_EXCEPTION(InvalidArgumentException, "Invalid personality type.\n");
	}
}

float Personality::operator[](PERSONALITY_TYPE type) const {
	return const_cast<Personality*>(this)->operator[](type);
}

float& Personality::operator()(const string& name) {
	if (name == "appearance") return appearance;
	if (name == "fitness") return fitness;
	if (name == "energy") return energy;
	if (name == "intelligence") return intelligence;
	if (name == "eloquence") return eloquence;
	if (name == "confidence") return confidence;
	if (name == "morality") return morality;
	if (name == "mentality") return mentality;
	if (name == "imagination") return imagination;
	if (name == "knowledge") return knowledge;
	if (name == "art") return art;
	if (name == "reasoning") return reasoning;
	if (name == "perception") return perception;
	THROW_EXCEPTION(InvalidArgumentException, "Invalid personality field name: " + name + ".\n");
}

float Personality::operator()(const string& name) const {
	return const_cast<Personality*>(this)->operator()(name);
}

string Personality::GetFieldName(PERSONALITY_TYPE type) {
	switch (type) {
	case PERSONALITY_APPEARANCE: return "appearance";
	case PERSONALITY_FITNESS: return "fitness";
	case PERSONALITY_ENERGY: return "energy";
	case PERSONALITY_INTELLIGENCE: return "intelligence";
	case PERSONALITY_ELOQUENCE: return "eloquence";
	case PERSONALITY_CONFIDENCE: return "confidence";
	case PERSONALITY_MORALITY: return "morality";
	case PERSONALITY_MENTALITY: return "mentality";
	case PERSONALITY_IMAGINATION: return "imagination";
	case PERSONALITY_KNOWLEDGE: return "knowledge";
	case PERSONALITY_ART: return "art";
	case PERSONALITY_REASONING: return "reasoning";
	case PERSONALITY_PERCEPTION: return "perception";
	default:
		THROW_EXCEPTION(InvalidArgumentException, "Invalid personality type.\n");
	}
}

Relation::Relation() :
	familiarity(0.f),
	respect(0.f),
	favour(0.f),
	trust(0.f),
	competing(0.f),
	reliability(0.f) {

}

float& Relation::operator[](RELATION_TYPE type) {
	switch (type) {
	case RELATION_FAMILIARITY: return familiarity;
	case RELATION_RESPECT: return respect;
	case RELATION_FAVOUR: return favour;
	case RELATION_TRUST: return trust;
	case RELATION_COMPETING: return competing;
	case RELATION_RELIABILITY: return reliability;
	default:
		THROW_EXCEPTION(InvalidArgumentException, "Invalid relation type.\n");
	}
}

float Relation::operator[](RELATION_TYPE type) const {
	return const_cast<Relation*>(this)->operator[](type);
}

float& Relation::operator()(const string& name) {
	if (name == "familiarity") return familiarity;
	if (name == "respect") return respect;
	if (name == "favour") return favour;
	if (name == "trust") return trust;
	if (name == "competing") return competing;
	if (name == "reliability") return reliability;
	THROW_EXCEPTION(InvalidArgumentException, "Invalid relation field name: " + name + ".\n");
}

float Relation::operator()(const string& name) const {
	return const_cast<Relation*>(this)->operator()(name);
}

string Relation::GetFieldName(RELATION_TYPE type) {
	switch (type) {
	case RELATION_FAMILIARITY: return "familiarity";
	case RELATION_RESPECT: return "respect";
	case RELATION_FAVOUR: return "favour";
	case RELATION_TRUST: return "trust";
	case RELATION_COMPETING: return "competing";
	case RELATION_RELIABILITY: return "reliability";
	default:
		THROW_EXCEPTION(InvalidArgumentException, "Invalid relation type.\n");
	}
}

Person::Person() :
	id(0),
	avatar(),
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
	personality(),
	acquaintances(),
	assets(),
	jobs(),
	home(nullptr),
	scheduler(nullptr),
	educationExperiences(),
	emotionExperiences(),
	jobExperiences(),
	options(),
	currentBlock(nullptr),
	currentZone(nullptr),
	currentBuilding(nullptr),
	currentRoom(nullptr),
	currentCommute(nullptr),
	statusChanged() {

}

Person::~Person() {
	for (auto& [_, asset] : assets) {
		if (asset) delete asset;
	}
	assets.clear();

	jobs.clear();

	if (scheduler)delete scheduler;
	scheduler = nullptr;

	if (currentCommute)delete currentCommute;
	currentCommute = nullptr;
}

int Person::GetId() const {
	return id;
}

void Person::SetId(int id) {
	this->id = id;
}

const string& Person::GetAvatar() const {
	return avatar;
}

void Person::SetAvatar(const string& avatar) {
	this->avatar = avatar;
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

bool Person::AddDeposit(int amount) {
	if (amount < 0 && deposit + amount < 0) {
		return false;
	}
	deposit += amount;
	return true;
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
	const string& key = asset->GetAsset().empty() ? asset->GetName() : asset->GetAsset();
	assets[key] = asset;
}

Asset* Person::GetAsset(const string& name) const {
	auto it = assets.find(name);
	return it != assets.end() ? it->second : nullptr;
}

void Person::RemoveAsset(const string& name) {
	auto it = assets.find(name);
	if (it == assets.end()) return;
	if (it->second) delete it->second;
	assets.erase(it);
}

const vector<Job*>& Person::GetJobs() const {
	return jobs;
}

void Person::AddJob(Job* job) {
	if (job == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Job is null.\n");
	}
	if (!job->GetScript()) {
		THROW_EXCEPTION(NullPointerException, "Job script is null.\n");
	}
	if (job->GetPosition()) {
		job->GetScript()->SetValue("self.work_address", job->GetPosition()->GetAddress());
	}
	if (home) {
		job->GetScript()->SetValue("self.home_address", home->GetAddress());
	}
	jobs.push_back(job);
}

void Person::RemoveJob(Job* job) {
	if (job == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Job is null.\n");
	}
	for (auto it = jobs.begin(); it != jobs.end(); ++it) {
		if (*it == job) {
			delete *it;
			jobs.erase(it);
			break;
		}
	}
}

Room* Person::GetHome() const {
	return home;
}

void Person::SetHome(Room* room) {
	home = room;
	if (scheduler) {
		scheduler->GetScript()->SetValue("self.home_address", room->GetAddress());
	}
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

const Personality& Person::GetPersonality() const {
	return personality;
}

void Person::SetPersonalityValue(PERSONALITY_TYPE type, float value) {
	personality[type] = value;
	if (scheduler) {
		scheduler->GetScript()->SetValue("self." + Personality::GetFieldName(type), static_cast<double>(value));
	}
}

void Person::AdjustPersonalityValue(PERSONALITY_TYPE type, float delta) {
	float value = clamp(personality[type] + delta, 0.0f, 1.0f);
	personality[type] = value;
	if (scheduler) {
		scheduler->GetScript()->SetValue("self." + Personality::GetFieldName(type), static_cast<double>(value));
	}
}

void Person::AddAcquaintance(const string& name) {
	acquaintances[name] = Relation();
	SetAcquaintanceValue(name, RELATION_FAMILIARITY, acquaintances[name][RELATION_FAMILIARITY]);
	SetAcquaintanceValue(name, RELATION_RESPECT, acquaintances[name][RELATION_RESPECT]);
	SetAcquaintanceValue(name, RELATION_FAVOUR, acquaintances[name][RELATION_FAVOUR]);
	SetAcquaintanceValue(name, RELATION_TRUST, acquaintances[name][RELATION_TRUST]);
	SetAcquaintanceValue(name, RELATION_COMPETING, acquaintances[name][RELATION_COMPETING]);
	SetAcquaintanceValue(name, RELATION_RELIABILITY, acquaintances[name][RELATION_RELIABILITY]);
}

const Relation& Person::GetAcquaintance(const string& name) const {
	auto it = acquaintances.find(name);
	if (it == acquaintances.end()) {
		THROW_EXCEPTION(InvalidArgumentException, "Acquaintance " + name + " not found.\n");
	}
	return it->second;
}

void Person::SetAcquaintanceValue(const string& name, RELATION_TYPE type, float value) {
	auto it = acquaintances.find(name);
	if (it == acquaintances.end()) {
		THROW_EXCEPTION(InvalidArgumentException, "Acquaintance " + name + " not found.\n");
	}
	it->second[type] = value;
	if (scheduler) {
		scheduler->GetScript()->SetValue("self.acquaintances." + name + "." + Relation::GetFieldName(type), static_cast<double>(value));
	}
}

void Person::AdjustAcquaintanceValue(const string& name, RELATION_TYPE type, float delta) {
	auto it = acquaintances.find(name);
	if (it == acquaintances.end()) {
		THROW_EXCEPTION(InvalidArgumentException, "Acquaintance " + name + " not found.\n");
	}
	float value = clamp(it->second[type] + delta, 0.0f, 1.0f);
	it->second[type] = value;
	if (scheduler) {
		scheduler->GetScript()->SetValue("self.acquaintances." + name + "." + Relation::GetFieldName(type), static_cast<double>(value));
	}
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

void Person::SetStatus(Zone* zone, bool notify) {
	if (zone == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Zone is null.\n");
	}
	currentBlock = zone->GetParent();
	currentZone = zone;
	currentBuilding = nullptr;
	currentRoom = nullptr;
	if (currentCommute)delete currentCommute;
	currentCommute = nullptr;
	
	if (notify)statusChanged = zone->GetAddress();
}

void Person::SetStatus(Building* building, bool notify) {
	if (building == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Building is null.\n");
	}
	currentBlock = building->GetParentBlock();
	currentZone = building->GetParentZone();
	currentBuilding = building;
	currentRoom = nullptr;
	if (currentCommute)delete currentCommute;
	currentCommute = nullptr;

	if (notify)statusChanged = building->GetAddress();
}

void Person::SetStatus(Room* room, bool notify) {
	if (room == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Room is null.\n");
	}
	currentBlock = room->GetParentBuilding()->GetParentBlock();
	currentZone = room->GetParentBuilding()->GetParentZone();
	currentBuilding = room->GetParentBuilding();
	currentRoom = room;
	if (currentCommute)delete currentCommute;
	currentCommute = nullptr;

	if (notify)statusChanged = room->GetAddress();
}

void Person::SetStatus(Room* target, const vector<pair<Connection*, bool>>& paths, const Time& time) {
	if (target == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Target room is null.\n");
	}
	currentBlock = nullptr;
	currentZone = nullptr;
	currentBuilding = nullptr;
	currentRoom = nullptr;
	currentCommute = new Commute();
	currentCommute->SetTarget(target->GetAddress());
	currentCommute->SetPaths(paths);
	currentCommute->SetTime(time);
}

Block* Person::GetCurrentBlock() const {
	return currentBlock;
}

Zone* Person::GetCurrentZone() const {
	return currentZone;
}

Building* Person::GetCurrentBuilding() const {
	return currentBuilding;
}

Room* Person::GetCurrentRoom() const {
	return currentRoom;
}

Commute* Person::GetCurrentCommute() const {
	return currentCommute;
}

string Person::PopChange() {
	string change = statusChanged;
	statusChanged = "";
	return change;
}

