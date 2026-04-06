#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Society {
public:
	// 构造社会
	Society();

	// 析构社会
	~Society();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取日程模组
	void InitCalendars(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取职业模组
	void InitJobs(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取组织模组
	void InitOrganizations(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化社区
	void Init(Map* map, Populace* populace, Player* player);

	// 释放空间
	void Destroy();

	// 时钟周期
	void Tick(Player* player);

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 日程工厂
	static CalendarFactory* calendarFactory;

	// 职业工厂
	static JobFactory* jobFactory;

	// 组织工厂
	static OrganizationFactory* organizationFactory;

private:
	// 全部组织
	OBJECT_HOLDER std::vector<Organization*> organizations;

};

