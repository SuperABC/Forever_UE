#include "experience.h"

using namespace std;

Experience::Experience() : begin(), end() {

}

Experience::~Experience() {

}

const Time& Experience::GetBeginTime() const {
	return begin;
}

const Time& Experience::GetEndTime() const {
	return end;
}

void Experience::SetTime(const Time& b, const Time& e) {
	begin = b;
	end = e;
}

EducationExperience::EducationExperience() :
	Experience(), school(), classmates(), teacher(nullptr), graduate(true) {

}

EducationExperience::~EducationExperience() {

}

const string& EducationExperience::GetSchool() const {
	return school;
}

void EducationExperience::SetSchool(const string& s) {
	school = s;
}

void EducationExperience::AddClassmate(Person* p) {
	if (p == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Classmate is null.\n");
	}
	classmates.push_back(p);
}

Person* EducationExperience::GetTeacher() const {
	return teacher;
}

void EducationExperience::SetTeacher(Person* t) {
	if (t == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Teacher is null.\n");
	}
	teacher = t;
}

const vector<Person*>& EducationExperience::GetClassmates() const {
	return classmates;
}

bool EducationExperience::GetGraduate() const {
	return graduate;
}

void EducationExperience::SetGraduate(bool g) {
	graduate = g;
}

EmotionExperience::EmotionExperience() :
	Experience(), person(nullptr) {

}

EmotionExperience::~EmotionExperience() {
}

Person* EmotionExperience::GetPerson() const {
	return person;
}

void EmotionExperience::SetPerson(Person* p) {
	if (p == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Emotion person is null.\n");
	}
	person = p;
}

JobExperience::JobExperience() :
	Experience(), organization(), colleagues() {

}

JobExperience::~JobExperience() {

}

const string& JobExperience::GetOrganization() const {
	return organization;
}

void JobExperience::SetOrganization(const string& o) {
	organization = o;
}

const vector<Person*>& JobExperience::GetColleagues() const {
	return colleagues;
}

void JobExperience::AddColleague(Person* p) {
	if (p == nullptr) {
		THROW_EXCEPTION(NullPointerException, "Colleague is null.\n");
	}
	colleagues.push_back(p);
}

