#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <set>
#include <tuple>


class Society {
public:
	/*
	* 构造社会
	*/
	Society();

	/*
	* 析构社会
	*/
	~Society();

	/*
	* 读取配置文件
	*/
	void LoadConfigs() const;

	/*
	* 读取日程模组
	* @modHandles, dlls: 模组句柄映射与动态库列表
	*/
	void InitCalendars(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取职业模组
	* @modHandles, dlls: 模组句柄映射与动态库列表
	*/
	void InitJobs(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取组织模组
	* @modHandles, dlls: 模组句柄映射与动态库列表
	*/
	void InitOrganizations(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 初始化社区
	* @map, populace, player: 地图、人口与玩家对象
	*/
	void Init(Map* map, Populace* populace, Player* player);

	/*
	* 释放空间
	*/
	void Destroy();

	/*
	* 时钟周期，返回本帧职业计划节点产生的变化列表（由调用方负责释放）
	* @player, story: 玩家与剧情对象
	*/
	std::vector<Change*> Tick(Player* player, Story* story);

	/*
	* 应用剧情变化
	* @change, getValues: 变化对象与值获取回调列表
	*/
	void ApplyChange(Populace* populace, Player* player, Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	/*
	* 按名称查找组织
	* @name: 组织实例名称
	*/
	Organization* GetOrganization(const std::string& name) const;

	/*
	* 获取全部组织列表
	*/
	const std::vector<Organization*>& GetOrganizations() const;

	// 日程工厂
	static CalendarFactory* calendarFactory;

	// 职业工厂
	static JobFactory* jobFactory;

	// 组织工厂
	static OrganizationFactory* organizationFactory;

private:
	// 全部组织
	OBJECT_HOLDER std::vector<Organization*> organizations;

	// 当前游戏时间（每次Tick更新，用于设置职业计划起始时间）
	Time currentTime;

	// 计划计时器集合（到达时间, Job*, Person*, Organization*, 节点名称），按到达时间有序
	// Job计时器：job/person非空，organization为nullptr
	// Organization计时器：organization非空，job/person为nullptr
	std::set<std::tuple<Time, Job*, Person*, Organization*, std::string>> timerSet;

};

