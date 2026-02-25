#pragma once

#include "../story/story.h"

#include "person.h"
#include "asset.h"
#include "job.h"
#include "name.h"
#include "scheduler.h"
#include "error.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Map;
class Story;
class Person;
class Event;

class Populace {
public:
	Populace();
	~Populace();

	// 设置资源路径
	void SetResourcePath(std::string path);

	// 读取Mods
	void InitAssets(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitJobs(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitNames(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitSchedulers(std::unordered_map<std::string, HMODULE>& modHandles);

	// 读取配置文件
	void ReadConfigs(std::string path) const;

	// 初始化全部人口
	void Init(int accomodation, std::vector<std::string> nameholders, Time *time);

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前人口
	void Print() const;

	// 保存/加载人口
	void Load(std::string path);
	void Save(std::string path) const;

	// 应用变更
	void ApplyChange(Change* change, Story* story,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	// 分配调度
	void Schedule() const;

	// 分配剧情
	void Workload(Story* story) const;
	void Characterize(std::string path, Story* story) const;

	// 获取市民
	std::vector<Person*>& GetCitizens();
	Person* GetCitizen(std::string name);

	// 获取Job工厂
	JobFactory* GetJobFactory();

	// 触发事件
	std::pair<std::vector<Dialog>, std::vector<Change*>> TriggerEvent(
		std::string name, Event* event, Story* story) const;
	std::pair<std::vector<Dialog>, std::vector<Change*>> TriggerEvent(
		int id, Event* event, Story* story) const;

private:
	// 生成市民
	void GenerateCitizens(int num, std::vector<std::string> nameholders, Time* time);
	void GenerateEducations(Time* time);
	void GenerateEmotions(Time* time);
	void GenerateJobs();

	// 资源路径
	std::string resourcePath;

	// Mod管理
	static AssetFactory* assetFactory;
	static JobFactory* jobFactory;
	static NameFactory* nameFactory;
	static SchedulerFactory* schedulerFactory;

	// 市民管理
	Name* names = nullptr;
	std::vector<Person*> citizens;
	std::unordered_map<std::string, int> ids;
};

