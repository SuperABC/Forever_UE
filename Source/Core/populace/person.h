#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#undef GetJob
#undef AddJob


enum GENDER_TYPE : int {
	GENDER_FEMALE, // 女性
	GENDER_MALE // 男性
};

enum RELATIVE_TYPE : int {
	RELATIVE_FATHER, // 父亲
	RELATIVE_MOTHER, // 母亲
	RELATIVE_WIFE, // 妻子
	RELATIVE_HUSBAND, // 丈夫
	RELATIVE_SON, // 儿子
	RELATIVE_DAUGHTER // 女儿
};

class Person {
public:
	// 构造空人物
	Person();

	// 析构人物
	~Person();

	// 获取标识符
	int GetId() const;

	// 设置标识符
	void SetId(int id);

	// 获取姓名
	const std::string& GetName() const;

	// 设置姓名
	void SetName(const std::string& name);

	// 获取性别
	GENDER_TYPE GetGender() const;

	// 设置性别
	void SetGender(GENDER_TYPE gender);

	// 获取身高
	float GetHeight() const;

	// 设置身高
	void SetHeight(float height);

	// 获取体重
	float GetWeight() const;

	// 设置体重
	void SetWeight(float weight);

	// 获取生日
	const Time& GetBirthday() const;

	// 设置生日
	void SetBirthday(const Time& birthday);

	// 获取年龄
	int GetAge(const Time* currentTime) const;

	// 获取结婚日期
	const Time& GetMarryday() const;

	// 设置结婚日期
	void SetMarryday(const Time& marryday);

	// 获取昵称
	const std::string& GetNick() const;

	// 设置昵称
	void SetNick(const std::string& nick);

	// 获取存款
	int GetDeposit() const;

	// 设置存款
	void SetDeposit(int deposit);

	// 获取手机号
	int GetPhone() const;

	// 设置手机号
	void SetPhone(int phone);

	// 添加亲属
	void AddRelative(RELATIVE_TYPE type, Person* person);

	// 获取父亲
	Person* GetFather() const;

	// 获取母亲
	Person* GetMother() const;

	// 获取配偶
	Person* GetSpouse() const;

	// 获取子女
	std::vector<Person*> GetChilds() const;

	// 添加资产
	void AddAsset(Asset* asset);

	// 获取全部资产
	std::vector<Asset*>& GetAssets();

	// 获取一类全部资产
	std::vector<Asset*> GetAssets(const std::string& type) const;

	// 按名称查找资产
	Asset* GetAsset(const std::string& name) const;

	// 获取全部工作
	std::vector<Job*> GetJobs() const;

	// 添加工作
	void AddJob(Job* job);

	// 移除工作
	void RemoveJob(Job* job);

	// 获取正在工作
	Job* GetWork() const;

	// 设置正在工作
	void SetWork(int job);

	// 获取居住房间
	Room* GetHome() const;

	// 设置居住房间
	void SetHome(Room* room);

	// 移除居住房间
	void RemoveHome();

	// 获取调度
	Scheduler* GetScheduler() const;

	// 设置调度
	void SetScheduler(Scheduler* scheduler);

	// 获取教育经历
	std::vector<EducationExperience>& GetEducationExperiences();

	// 添加教育经历
	void AddEducationExperience(EducationExperience exp);

	// 获取感情经历
	std::vector<EmotionExperience>& GetEmotionExperiences();

	// 添加感情经历
	void AddEmotionExperience(EmotionExperience exp);

	// 获取工作经历
	std::vector<JobExperience>& GetJobExperiences();

	// 添加工作经历
	void AddJobExperience(JobExperience exp);

	// 相同经历合并
	void ExperienceComposition();

	// 添加选项
	bool AddOption(const std::string& option);

	// 移除选项
	bool RemoveOption(const std::string& option);

	// 获取选项
	std::unordered_set<std::string> GetOptions() const;

	// 设置所在园区
	//void SetStatus(Zone* zone);

	// 设置所在建筑
	//void SetStatus(Building* building);

	// 设置所在房间
	//void SetStatus(Room* room);

	// 设置通勤
	//void SetStatus(Room* target, std::vector<Connection> paths, Time time);

	// 获取所在地块
	//Block* GetCurrentBlock() const;

	// 获取所在园区
	//Zone* GetCurrentZone() const;

	// 获取所在建筑
	//Building* GetCurrentBuilding() const;

	// 获取所在房间
	//Room* GetCurrentRoom() const;

	// 获取通勤
	//Commute GetCurrentCommute() const;

private:
	int id;
	std::string name;
	GENDER_TYPE gender;
	Time birthday;
	Time marryday;
	float height;
	float weight;
	std::string nick;
	int deposit;
	int phone;

	std::vector<std::pair<RELATIVE_TYPE, Person*>> relatives;
	std::vector<std::pair<std::string, Person*>> acquaintances;

	OBJECT_HOLDER std::vector<Asset*> assets;

	OBJECT_HOLDER std::vector<Job*> jobs;
	int working;

	Room* home;
	OBJECT_HOLDER Scheduler* scheduler;

	std::vector<EducationExperience> educationExperiences;
	std::vector<EmotionExperience> emotionExperiences;
	std::vector<JobExperience> jobExperiences;

	std::unordered_set<std::string> options;

	//Plot* currentPlot;
	//Zone* currentZone;
	//Building* currentBuilding;
	//Room* currentRoom;
	//Commute commute;
};

