#pragma once

#include "story/story.h"
#include "player/player.h"

#include "person.h"
#include "asset.h"
#include "name.h"
#include "scheduler.h"
#include "condition.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <string>
#include <vector>
#include <functional>

class Person;
class Change;
class Player;
class Time;
class Asset;
class Name;
class Scheduler;
class AssetFactory;
class NameFactory;
class SchedulerFactory;
class Populace {
public:
	Populace();
	~Populace();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取 Mods
	void InitAssets(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitNames(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);
	void InitSchedulers(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化人口
	void Init(int accomodation, Player* player);

	// 释放空间
	void Destroy();

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 获取全部市民
	std::vector<Person*>& GetCitizens();

	// 按姓名查找市民
	Person* GetCitizen(const std::string& name);

	// 统一工厂
	static AssetFactory* assetFactory;
	static NameFactory* nameFactory;
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
	Name* name;

	// 全部市民
	OBJECT_HOLDER std::vector<Person*> citizens;

	// 身份证号
	std::unordered_map<std::string, int> ids;
};
