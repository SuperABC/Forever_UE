#include "experience.h"


using namespace std;

Experience::Experience() :
	begin(),
	end() {

}

Experience::~Experience() {

}

const Time& Experience::GetBeginTime() const {
	return begin;
}

const Time& Experience::GetEndTime() const {
	return end;
}

void Experience::SetTime(const Time& beginTime, const Time& endTime) {
	begin = beginTime;
	end = endTime;
}

EducationExperience::EducationExperience() :
	Experience(),
	school(),
	classmates(),
	teacher(nullptr),
	graduate(true) {

}

EducationExperience::~EducationExperience() {

}

const string& EducationExperience::GetSchool() const {
	return school;
}

void EducationExperience::SetSchool(const string& school) {
	this->school = school;
}

void EducationExperience::AddClassmate(Person* classmate) {
	if (classmate == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Classmate is null.\n");
	}
	classmates.push_back(classmate);
}

Person* EducationExperience::GetTeacher() const {
	return teacher;
}

void EducationExperience::SetTeacher(Person* teacher) {
	if (teacher == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Teacher is null.\n");
	}
	this->teacher = teacher;
}

const vector<Person*>& EducationExperience::GetClassmates() const {
	return classmates;
}

bool EducationExperience::GetGraduate() const {
	return graduate;
}

void EducationExperience::SetGraduate(bool graduate) {
	this->graduate = graduate;
}

EmotionExperience::EmotionExperience() :
	Experience(),
	person(nullptr) {

}

EmotionExperience::~EmotionExperience() {
}

Person* EmotionExperience::GetPerson() const {
	return person;
}

void EmotionExperience::SetPerson(Person* person) {
	if (person == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Emotion person is null.\n");
	}
	this->person = person;
}

JobExperience::JobExperience() :
	Experience(),
	organization(),
	colleagues() {

}

JobExperience::~JobExperience() {

}

const string& JobExperience::GetOrganization() const {
	return organization;
}

void JobExperience::SetOrganization(const string& organization) {
	this->organization = organization;
}

const vector<Person*>& JobExperience::GetColleagues() const {
	return colleagues;
}

void JobExperience::AddColleague(Person* colleague) {
	if (colleague == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Colleague is null.\n");
	}
	colleagues.push_back(colleague);
}

