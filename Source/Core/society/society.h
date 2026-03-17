#pragma once

#include "../map/map.h"
#include "../populace/populace.h"
#include "../story/story.h"

#include "job.h"
#include "calendar.h"
#include "organization.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Society {
public:
	Society();
	~Society();

	// 读取Mods
	void InitJobs(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitCalendars(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);
	void InitOrganizations(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);

	// 读取配置文件
	void LoadConfigs() const;

	// 初始化全部组织
	void Init(Map* map, Populace* populace, Time* time);

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前组织
	void Print() const;

	// 保存/加载组织
	void Load(const std::string& path);
	void Save(const std::string& path) const;

	// 应用变更
	void ApplyChange(Change* change, Story* story,
		std::vector<std::function<std::pair<bool, ValueType>(
			const std::string&)>>&getValues);

private:
	// Mod管理
	static JobFactory* jobFactory;
	static CalendarFactory* calendarFactory;
	static OrganizationFactory* organizationFactory;

	std::vector<Organization*> organizations;
};

