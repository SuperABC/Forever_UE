#pragma once

#include "person.h"


class Person;

class Experience {
public:
    Experience() = default;
    virtual ~Experience() = default;

    // 获取/设置经历时间
    const Time& GetBeginTime() const { return begin; }
    const Time& GetEndTime() const { return end; }
    void SetTime(const Time& b, const Time& e) { begin = b; end = e; }

private:
    Time begin, end;
};

class EducationExperience : public Experience {
public:
    EducationExperience() = default;

    // 设置教育经历属性
    void SetSchool(const std::string& s) { school = s; }
    const std::string& GetSchool() const { return school; }
    void AddClassmate(Person* p) { classmates.push_back(p); }
    void SetTeacher(Person* t) { teacher = t; }
    const std::vector<Person*>& GetClassmates() const { return classmates; }
    Person* GetTeacher() const { return teacher; }
    void SetGraduate(bool g) { graduate = g; }
    bool GetGraduate() const { return graduate; }

private:
    std::string school;
    std::vector<Person*> classmates;
    Person* teacher = nullptr;

    bool graduate = true;
};

class EmotionExperience : public Experience {
public:
    EmotionExperience() = default;

    // 获取/设置情感经历属性
    Person* GetPerson() const { return person; }
    void SetPerson(Person* p) { person = p; }

private:
    Person* person;
};


class JobExperience : public Experience {
public:
    JobExperience() = default;

    // 获取/设置工作经历属性
    void SetOrganization(std::string o) { organization = o; }
    std::string GetOrganization() const { return organization; }
    void AddColleague(Person* p) { colleagues.push_back(p); }
    const std::vector<Person*>& GetColleagues() const { return colleagues; }

private:
    std::string organization;
    std::vector<Person*> colleagues;
};
