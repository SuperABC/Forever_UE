#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <set>
#include <tuple>


class Populace {
public:
	/*
	* 构造人口
	*/
	Populace();

	/*
	* 析构人口
	*/
	~Populace();

	/*
	* 读取配置文件
	*/
	void LoadConfigs() const;

	/*
	* 读取姓名模组
	* @modHandles, dlls: 模组句柄映射与动态库列表
	*/
	void InitNames(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取调度模组
	* @modHandles, dlls: 模组句柄映射与动态库列表
	*/
	void InitSchedulers(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 初始化人口
	* @accomodation, player: 容纳量与玩家对象
	* @post: 向Core发起查询的句柄
	*/
	void Init(int accomodation, Player* player, PostHandle* post);

	/*
	* 释放空间
	*/
	void Destroy();

	/*
	* 时钟周期，返回本帧调度节点产生的变化列表（由调用方负责释放）
	* @map, story, player: 地图、剧情、玩家
	* @post: 向Core发起查询的句柄
	* @maxTimers: 本次最多执行的到期调度节点数量，默认按正常帧率节流，剧情跳变时间时可传更大值一次性执行完
	*/
	std::vector<std::pair<Change*, Script*>> Tick(Map* map, Story* story, Player* player, PostHandle* post, int maxTimers = 4);

	/*
	* 应用剧情变化
	* @map, change, getValues: 地图、变化对象、值获取回调列表
	*/
	std::vector<Event*> ApplyChange(Map* map, Player* player, Traffic* traffic, const Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	/*
	* 获取全部市民
	*/
	std::vector<Person*>& GetCitizens();

	/*
	* 按姓名查找市民
	* @name: 市民姓名
	*/
	Person* GetCitizen(const std::string& name) const;

	/*
	* 按精细程度从房间到地块依次取第一个可用的导航节点
	* @room, building, zone, block: 候选位置，按精细程度从高到低传入
	*/
	static Node* ResolveNavigationNode(Room* room, Building* building, Zone* zone, Block* block);

	static NameFactory* nameFactory; // 姓名工厂

	static SchedulerFactory* schedulerFactory; // 调度工厂

private:
	/*
	* 生成市民
	* @num, time: 数量与游戏时间指针
	* @post: 向Core发起查询的句柄
	*/
	void GenerateCitizens(int num, Time* time, PostHandle* post);

	/*
	* 生成教育经历
	* @time: 游戏时间指针
	*/
	void GenerateEducations(Time* time);

	/*
	* 生成感情经历
	* @time: 游戏时间指针
	*/
	void GenerateEmotions(Time* time);

	/*
	* 生成工作经历
	*/
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
