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

// 成年年龄阈值
#define ADULT_AGE 18


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
	/*
	* 构造空人物
	*/
	Person();

	/*
	* 析构人物
	*/
	~Person();

	/*
	* 获取标识符
	*/
	int GetId() const;

	/*
	* 设置标识符
	* @id: 标识符
	*/
	void SetId(int id);

	/*
	* 获取头像
	*/
	const std::string& GetAvatar() const;

	/*
	* 设置头像
	* @avatar: 头像路径
	*/
	void SetAvatar(const std::string& avatar);

	/*
	* 获取姓名
	*/
	const std::string& GetName() const;

	/*
	* 设置姓名
	* @name: 姓名
	*/
	void SetName(const std::string& name);

	/*
	* 获取性别
	*/
	GENDER_TYPE GetGender() const;

	/*
	* 设置性别
	* @gender: 性别
	*/
	void SetGender(GENDER_TYPE gender);

	/*
	* 获取身高
	*/
	float GetHeight() const;

	/*
	* 设置身高
	* @height: 身高
	*/
	void SetHeight(float height);

	/*
	* 获取体重
	*/
	float GetWeight() const;

	/*
	* 设置体重
	* @weight: 体重
	*/
	void SetWeight(float weight);

	/*
	* 获取生日
	*/
	const Time& GetBirthday() const;

	/*
	* 设置生日
	* @birthday: 生日
	*/
	void SetBirthday(const Time& birthday);

	/*
	* 获取年龄
	* @currentTime: 当前时间指针
	*/
	int GetAge(const Time* currentTime) const;

	/*
	* 获取结婚日期
	*/
	const Time& GetMarryday() const;

	/*
	* 设置结婚日期
	* @marryday: 结婚日期
	*/
	void SetMarryday(const Time& marryday);

	/*
	* 获取昵称
	*/
	const std::string& GetNick() const;

	/*
	* 设置昵称
	* @nick: 昵称
	*/
	void SetNick(const std::string& nick);

	/*
	* 获取存款
	*/
	int GetDeposit() const;

	/*
	* 设置存款
	* @deposit: 存款金额
	*/
	void SetDeposit(int deposit);

	/*
	* 获取手机号
	*/
	int GetPhone() const;

	/*
	* 设置手机号
	* @phone: 手机号
	*/
	void SetPhone(int phone);

	/*
	* 添加亲属
	* @type, person: 亲属类型与对象
	*/
	void AddRelative(RELATIVE_TYPE type, Person* person);

	/*
	* 获取父亲
	*/
	Person* GetFather() const;

	/*
	* 获取母亲
	*/
	Person* GetMother() const;

	/*
	* 获取配偶
	*/
	Person* GetSpouse() const;

	/*
	* 获取子女列表
	*/
	std::vector<Person*> GetChilds() const;

	/*
	* 添加资产
	* @asset: 资产对象
	*/
	void AddAsset(Asset* asset);

	/*
	* 获取全部资产
	*/
	std::vector<Asset*>& GetAssets();

	/*
	* 获取一类全部资产
	* @type: 资产类型
	*/
	std::vector<Asset*> GetAssets(const std::string& type) const;

	/*
	* 按名称查找资产
	* @name: 资产名称
	*/
	Asset* GetAsset(const std::string& name) const;

	/*
	* 获取全部工作
	*/
	const std::vector<Job*>& GetJobs() const;

	/*
	* 添加工作
	* @job: 工作对象
	*/
	void AddJob(Job* job);

	/*
	* 移除工作
	* @job: 工作对象
	*/
	void RemoveJob(Job* job);

	/*
	* 获取居住房间
	*/
	Room* GetHome() const;

	/*
	* 设置居住房间
	* @room: 房间对象
	*/
	void SetHome(Room* room);

	/*
	* 移除居住房间
	*/
	void RemoveHome();

	/*
	* 获取调度
	*/
	Scheduler* GetScheduler() const;

	/*
	* 设置调度
	* @scheduler: 调度对象
	*/
	void SetScheduler(Scheduler* scheduler);

	/*
	* 获取教育经历列表
	*/
	std::vector<EducationExperience>& GetEducationExperiences();

	/*
	* 添加教育经历
	* @exp: 教育经历
	*/
	void AddEducationExperience(EducationExperience exp);

	/*
	* 获取感情经历列表
	*/
	std::vector<EmotionExperience>& GetEmotionExperiences();

	/*
	* 添加感情经历
	* @exp: 感情经历
	*/
	void AddEmotionExperience(EmotionExperience exp);

	/*
	* 获取工作经历列表
	*/
	std::vector<JobExperience>& GetJobExperiences();

	/*
	* 添加工作经历
	* @exp: 工作经历
	*/
	void AddJobExperience(JobExperience exp);

	/*
	* 相同经历合并
	*/
	void ExperienceComposition();

	/*
	* 添加选项
	* @option: 选项名称
	*/
	bool AddOption(const std::string& option);

	/*
	* 移除选项
	* @option: 选项名称
	*/
	bool RemoveOption(const std::string& option);

	/*
	* 获取全部选项
	*/
	std::unordered_set<std::string> GetOptions() const;

	/*
	* 设置所在园区
	* @zone: 园区对象
	*/
	void SetStatus(Zone* zone);

	/*
	* 设置所在建筑
	* @building: 建筑对象
	*/
	void SetStatus(Building* building);

	/*
	* 设置所在房间
	* @room: 房间对象
	*/
	void SetStatus(Room* room);

	/*
	* 设置通勤
	* @target, paths, time: 目标房间、路线及出发时间
	*/
	void SetStatus(Room* target, const std::vector<Connection*>& paths, const Time& time);

	/*
	* 获取所在地块
	*/
	Block* GetCurrentBlock() const;

	/*
	* 获取所在园区
	*/
	Zone* GetCurrentZone() const;

	/*
	* 获取所在建筑
	*/
	Building* GetCurrentBuilding() const;

	/*
	* 获取所在房间
	*/
	Room* GetCurrentRoom() const;

	/*
	* 获取通勤
	*/
	Commute* GetCurrentCommute() const;

	/*
	* 读取并重置状态变化
	*/
	bool PopChange();

private:
	// 标识符
	int id;

	// 头像路径
	std::string avatar;

	// 姓名
	std::string name;

	// 性别
	GENDER_TYPE gender;

	// 生日
	Time birthday;

	// 结婚日期
	Time marryday;

	// 身高
	float height;

	// 体重
	float weight;

	// 昵称
	std::string nick;

	// 存款
	int deposit;

	// 手机号
	int phone;

	// 亲属列表
	std::vector<std::pair<RELATIVE_TYPE, Person*>> relatives;

	// 熟人列表
	std::vector<std::pair<std::string, Person*>> acquaintances;

	// 持有资产列表
	OBJECT_HOLDER std::vector<Asset*> assets;

	// 居住房间
	Room* home;

	// 工作列表
	std::vector<Job*> jobs;

	// 调度对象
	OBJECT_HOLDER Scheduler* scheduler;

	// 教育经历列表
	std::vector<EducationExperience> educationExperiences;

	// 感情经历列表
	std::vector<EmotionExperience> emotionExperiences;

	// 工作经历列表
	std::vector<JobExperience> jobExperiences;

	// 选项集合
	std::unordered_set<std::string> options;

	// 当前所在地块
	Block* currentBlock;

	// 当前所在园区
	Zone* currentZone;

	// 当前所在建筑
	Building* currentBuilding;

	// 当前所在房间
	Room* currentRoom;

	// 当前通勤
	OBJECT_HOLDER Commute* currentCommute;

	// 状态是否已变化
	bool statusChanged;
};
