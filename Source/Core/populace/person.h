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

enum PERSONALITY_TYPE : int {
	PERSONALITY_APPEARANCE, // 颜值
	PERSONALITY_FITNESS, // 身材
	PERSONALITY_ENERGY, // 体能
	PERSONALITY_INTELLIGENCE, // 智商
	PERSONALITY_ELOQUENCE, // 口才
	PERSONALITY_CONFIDENCE, // 自信心
	PERSONALITY_MORALITY, // 德行
	PERSONALITY_MENTALITY, // 心态
	PERSONALITY_IMAGINATION, // 想象力
	PERSONALITY_KNOWLEDGE, // 学识
	PERSONALITY_ART, // 艺术
	PERSONALITY_REASONING, // 理性
	PERSONALITY_PERCEPTION // 感性
};

enum RELATION_TYPE : int {
	RELATION_FAMILIARITY, // 熟悉程度
	RELATION_RESPECT, // 尊敬程度
	RELATION_FAVOUR, // 好感程度
	RELATION_TRUST, // 信任程度
	RELATION_COMPETING, // 竞争程度
	RELATION_RELIABILITY // 依赖程度
};

// 个人属性
struct Personality {
	/*
	* 构造个人属性，各项属性随机初始化为[0, 1)区间内的值
	*/
	Personality();

	/*
	* 按类型访问属性字段（可写）
	* @type: 属性类型
	* @return: 字段引用
	*/
	float& operator[](PERSONALITY_TYPE type);

	/*
	* 按类型访问属性字段（只读）
	* @type: 属性类型
	* @return: 字段值
	*/
	float operator[](PERSONALITY_TYPE type) const;

	/*
	* 按脚本变量名访问属性字段（可写）
	* @name: 脚本变量名
	* @return: 字段引用
	*/
	float& operator()(const std::string& name);

	/*
	* 按脚本变量名访问属性字段（只读）
	* @name: 脚本变量名
	* @return: 字段值
	*/
	float operator()(const std::string& name) const;

	/*
	* 获取属性类型对应的脚本变量名
	* @type: 属性类型
	* @return: 脚本变量名
	*/
	static std::string GetFieldName(PERSONALITY_TYPE type);

	// 颜值
	float appearance;

	// 身材
	float fitness;

	// 体能
	float energy;

	// 智商
	float intelligence;

	// 口才
	float eloquence;

	// 自信心
	float confidence;

	// 德行
	float morality;

	// 心态
	float mentality;

	// 想象力
	float imagination;

	// 学识
	float knowledge;

	// 艺术
	float art;

	// 理性
	float reasoning;

	// 感性
	float perception;
};

// 人际关系
struct Relation {
	/*
	* 构造人际关系，各项属性初始化为0
	*/
	Relation();

	/*
	* 按类型访问关系字段（可写）
	* @type: 关系类型
	* @return: 字段引用
	*/
	float& operator[](RELATION_TYPE type);

	/*
	* 按类型访问关系字段（只读）
	* @type: 关系类型
	* @return: 字段值
	*/
	float operator[](RELATION_TYPE type) const;

	/*
	* 按脚本变量名访问关系字段（可写）
	* @name: 脚本变量名
	* @return: 字段引用
	*/
	float& operator()(const std::string& name);

	/*
	* 按脚本变量名访问关系字段（只读）
	* @name: 脚本变量名
	* @return: 字段值
	*/
	float operator()(const std::string& name) const;

	/*
	* 获取关系类型对应的脚本变量名
	* @type: 关系类型
	* @return: 脚本变量名
	*/
	static std::string GetFieldName(RELATION_TYPE type);

	// 熟悉程度
	float familiarity;

	// 尊敬程度
	float respect;

	// 好感程度
	float favour;

	// 信任程度
	float trust;

	// 竞争程度
	float competing;

	// 依赖程度
	float reliability;
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
	* 存取款，扣款时若余额不足则不执行并返回 false
	* @amount: 金额（正数存入，负数取出）
	*/
	bool AddDeposit(int amount);

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
	* 按名称查找资产
	* @name: 资产名称
	*/
	Asset* GetAsset(const std::string& name) const;

	/*
	* 按名称移除并销毁资产
	* @name: 资产名称
	*/
	void RemoveAsset(const std::string& name);

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
	* 获取个人属性
	*/
	const Personality& GetPersonality() const;

	/*
	* 设置个人属性中指定类型的绝对值，并写入脚本变量
	* @type, value: 属性类型与属性值
	*/
	void SetPersonalityValue(PERSONALITY_TYPE type, float value);

	/*
	* 按增量调整个人属性中指定类型的值，结果限制在[0, 1]区间内，并写入脚本变量
	* @type, delta: 属性类型与增量
	*/
	void AdjustPersonalityValue(PERSONALITY_TYPE type, float delta);

	/*
	* 添加熟人关系，初始为默认值并写入脚本变量
	* @name: 熟人姓名
	*/
	void AddAcquaintance(const std::string& name);

	/*
	* 获取指定熟人的关系，不存在时抛出异常
	* @name: 熟人姓名
	*/
	const Relation& GetAcquaintance(const std::string& name) const;

	/*
	* 设置指定熟人关系中指定类型的绝对值，并写入脚本变量
	* @name, type, value: 熟人姓名、关系类型与关系值
	*/
	void SetAcquaintanceValue(const std::string& name, RELATION_TYPE type, float value);

	/*
	* 按增量调整指定熟人关系中指定类型的值，结果限制在[0, 1]区间内，并写入脚本变量
	* @name, type, delta: 熟人姓名、关系类型与增量
	*/
	void AdjustAcquaintanceValue(const std::string& name, RELATION_TYPE type, float delta);

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
	* @notify: 是否记录变化
	*/
	void SetStatus(Zone* zone, bool notify = true);

	/*
	* 设置所在建筑
	* @building: 建筑对象
	* @notify: 是否记录变化
	*/
	void SetStatus(Building* building, bool notify = true);

	/*
	* 设置所在房间
	* @room: 房间对象
	* @notify: 是否记录变化
	*/
	void SetStatus(Room* room, bool notify = true);

	/*
	* 设置通勤
	* @target, paths, time: 目标房间、路线及出发时间
	*/
	void SetStatus(Room* target, const std::vector<std::pair<Connection*, bool>>& paths, const Time& time);

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
	std::string PopChange();

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

	// 个人属性
	Personality personality;

	// 熟人关系表
	std::unordered_map<std::string, Relation> acquaintances;

	// 持有房产与载具资产列表
	OBJECT_HOLDER std::unordered_map<std::string, Asset*> assets;

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

	// 位置状态是否已变化
	std::string statusChanged;
};
