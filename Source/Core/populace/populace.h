#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <set>
#include <tuple>


class Populace {
public:
	// 构造人口
	Populace();

	// 析构人口
	~Populace();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取姓名模组
	void InitNames(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取调度模组
	void InitSchedulers(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化人口
	void Init(int accomodation, Player* player);

	// 释放空间
	void Destroy();

	/*
	* 时钟周期，返回本帧调度节点产生的变化列表（由调用方负责释放）
	* @map, story, player: 地图、剧情、玩家
	*/
	std::vector<Change*> Tick(Map* map, Story* story, Player* player);

	// 应用剧情变化
	void ApplyChange(Map* map, Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	// 获取全部市民
	std::vector<Person*>& GetCitizens();

	// 按姓名查找市民
	Person* GetCitizen(const std::string& name);

	// 姓名工厂
	static NameFactory* nameFactory;

	// 调度工厂
	static SchedulerFactory* schedulerFactory;

private:
	// 生成市民
	void GenerateCitizens(int num, Time* time);

	// 生成教育经历
	void GenerateEducations(Time* time);

	// 生成感情经历
	void GenerateEmotions(Time* time);

	// 生成工作经历
	void GenerateJobs();

	// 姓名生成器
	OBJECT_HOLDER Name* name;

	// 全部市民
	OBJECT_HOLDER std::vector<Person*> citizens;

	// 身份证号（姓名->所在citizens索引）
	std::unordered_map<std::string, int> ids;

	// 当前游戏时间（每次Tick更新，用于设置通勤起始时间）
	Time currentTime;

	// 市民计划计时器集合（到达时间, Person*, 节点名称），按到达时间有序
	std::set<std::tuple<Time, Person*, std::string>> timerSet;
};
