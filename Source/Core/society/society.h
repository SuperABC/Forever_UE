#pragma once

#include "story/story.h"

#include "calendar.h"
#include "job.h"
#include "organization.h"
#include "condition.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <string>
#include <vector>
#include <functional>


class Society {
public:
	Society();
	~Society();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取 Mods
	void InitCalendars(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitJobs(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化社区
	void Init();

	// 释放空间
	void Destroy();

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 统一工厂
	static CalendarFactory* calendarFactory;
	static JobFactory* jobFactory;

private:

};

