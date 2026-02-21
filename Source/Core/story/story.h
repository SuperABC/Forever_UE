#pragma once

#include "../populace/populace.h"

#include "utility.h"
#include "json.h"
#include "script.h"
#include "dialog.h"
#include "event.h"
#include "change.h"
#include "milestone.h"
#include "condition.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Script;
class Person;
class Event;
class Change;
class EventFactory;
class ChangeFactory;

class Story {
public:
	Story();
	~Story();

	// 设置资源路径
	void SetResourcePath(std::string path);

	// 读取Mods
	void InitEvents(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitChanges(std::unordered_map<std::string, HMODULE>& modHandles);


	// 读取配置文件
	void ReadConfigs(std::string path) const;

	// 初始化全部剧情
	void Init();

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前剧情
	void Print() const;

	// 保存/读取剧情
	void Load(std::string path);
	void Save(std::string path) const;

	// 应用变更
	void ApplyChange(Change* change, Story* story,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	// 获取工厂
	EventFactory* GetEventFactory();
	ChangeFactory* GetChangeFactory();

	// 读取剧本
	std::vector<std::string> ReadNames(std::string name, std::string path) const;
	void ReadStory(std::string name, std::string path);

	// 判断条件
	bool JudgeCondition(Condition& condition) const;
	bool JudgeCondition(Condition& condition, Person* person) const;
	bool JudgeCondition(Condition& condition, std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues) const;

	// 匹配事件
	std::pair<std::vector<Dialog>, std::vector<Change*>> MatchEvent(Event* event);

	// 变量管理
	void InitVariables(Time *t);
	void SetValue(const std::string& name, ValueType value);
	std::pair<bool, ValueType> GetValue(const std::string& name) const;

private:
	// 变量管理
	void UpdateVariables(Time* t);

	// 资源路径
	std::string resourcePath;

	// Mod管理
	static EventFactory* eventFactory;
	static ChangeFactory* changeFactory;

	Script* script = nullptr;
	std::unordered_map<std::string, ValueType> variables;

};

