#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include "person.h"


class Experience {
public:
	/*
	* 构造经历
	*/
	Experience();

	/*
	* 析构经历
	*/
	virtual ~Experience();

	/*
	* 获取开始时间
	*/
	const Time& GetBeginTime() const;

	/*
	* 获取结束时间
	*/
	const Time& GetEndTime() const;

	/*
	* 设置时间段
	* @beginTime, endTime: 开始时间与结束时间
	*/
	void SetTime(const Time& beginTime, const Time& endTime);

private:
	// 开始时间
	Time begin;

	// 结束时间
	Time end;
};

class EducationExperience : public Experience {
public:
	/*
	* 构造教育经历
	*/
	EducationExperience();

	/*
	* 析构教育经历
	*/
	virtual ~EducationExperience() override;

	/*
	* 获取学校名称
	*/
	const std::string& GetSchool() const;

	/*
	* 设置学校名称
	* @school: 学校名称
	*/
	void SetSchool(const std::string& school);

	/*
	* 添加同学
	* @classmate: 同学对象
	*/
	void AddClassmate(Person* classmate);

	/*
	* 获取教师
	*/
	Person* GetTeacher() const;

	/*
	* 设置教师
	* @teacher: 教师对象
	*/
	void SetTeacher(Person* teacher);

	/*
	* 获取同学列表
	*/
	const std::vector<Person*>& GetClassmates() const;

	/*
	* 获取是否毕业
	*/
	bool GetGraduate() const;

	/*
	* 设置是否毕业
	* @graduate: 是否毕业
	*/
	void SetGraduate(bool graduate);

private:
	// 学校名称
	std::string school;

	// 同学列表
	std::vector<Person*> classmates;

	// 教师
	Person* teacher;

	// 是否毕业
	bool graduate;
};

class EmotionExperience : public Experience {
public:
	/*
	* 构造感情经历
	*/
	EmotionExperience();

	/*
	* 析构感情经历
	*/
	virtual ~EmotionExperience() override;

	/*
	* 获取相关人员
	*/
	Person* GetPerson() const;

	/*
	* 设置相关人员
	* @person: 相关人员对象
	*/
	void SetPerson(Person* person);

private:
	// 相关人员
	Person* person;
};

class JobExperience : public Experience {
public:
	/*
	* 构造工作经历
	*/
	JobExperience();

	/*
	* 析构工作经历
	*/
	virtual ~JobExperience() override;

	/*
	* 获取所在机构名称
	*/
	const std::string& GetOrganization() const;

	/*
	* 设置所在机构名称
	* @organization: 机构名称
	*/
	void SetOrganization(const std::string& organization);

	/*
	* 获取同事列表
	*/
	const std::vector<Person*>& GetColleagues() const;

	/*
	* 添加同事
	* @colleague: 同事对象
	*/
	void AddColleague(Person* colleague);

private:
	// 所在机构名称
	std::string organization;

	// 同事列表
	std::vector<Person*> colleagues;
};
