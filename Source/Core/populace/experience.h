#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include "person.h"


class Experience {
public:
	Experience();
	virtual ~Experience();

	const Time& GetBeginTime() const;
	const Time& GetEndTime() const;
	void SetTime(const Time& b, const Time& e);

private:
	Time begin;
	Time end;
};

class EducationExperience : public Experience {
public:
	EducationExperience();
	virtual ~EducationExperience() override;

	const std::string& GetSchool() const;
	void SetSchool(const std::string& s);
	void AddClassmate(Person* p);
	Person* GetTeacher() const;
	void SetTeacher(Person* t);
	const std::vector<Person*>& GetClassmates() const;
	bool GetGraduate() const;
	void SetGraduate(bool g);

private:
	std::string school;
	std::vector<Person*> classmates;
	Person* teacher;
	bool graduate;
};

class EmotionExperience : public Experience {
public:
	EmotionExperience();
	virtual ~EmotionExperience() override;

	Person* GetPerson() const;
	void SetPerson(Person* p);

private:
	Person* person;
};

class JobExperience : public Experience {
public:
	JobExperience();
	virtual ~JobExperience() override;

	const std::string& GetOrganization() const;
	void SetOrganization(const std::string& o);
	const std::vector<Person*>& GetColleagues() const;
	void AddColleague(Person* p);

private:
	std::string organization;
	std::vector<Person*> colleagues;
};

