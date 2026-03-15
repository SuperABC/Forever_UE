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
	void SetResourcePath(const std::string& path);

	// 读取Mods
	void InitEvents(std::unordered_map<std::string, HMODULE>& modHandles);
	void InitChanges(std::unordered_map<std::string, HMODULE>& modHandles);

	// 读取配置文件
	void ReadConfigs(const std::string& path) const;

	// 初始化全部剧情
	void Init();

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前剧情
	void Print() const;

	// 保存/读取剧情
	void Load(const std::string& path);
	void Save(const std::string& path) const;

	// 应用变更
	void ApplyChange(Change* change, Story* story,
		std::vector<std::function<std::pair<bool, ValueType>(
			const std::string&)>>&getValues);

	// 获取工厂
	EventFactory* GetEventFactory() const;
	ChangeFactory* GetChangeFactory() const;

	// 读取剧本
	std::vector<std::string> ReadNames(const std::string& name,
		const std::string& path) const;
	void ReadStory(const std::string& name, const std::string& path);

	// 判断条件
	bool JudgeCondition(const Condition& condition) const;
	bool JudgeCondition(const Condition& condition,
		const Person* person) const;
	bool JudgeCondition(const Condition& condition,
		std::vector<std::function<std::pair<bool, ValueType>(
			const std::string&)>> getValues) const;

	// 匹配事件
	std::pair<std::vector<Dialog>, std::vector<Change*>> MatchEvent(
		Event* event);

	// 变量管理
	std::pair<bool, ValueType> GetValue(const std::string& name) const;
	void SetValue(const std::string& name, ValueType value);
	void InitVariables(Time* t);

private:
	// 变量管理
	void UpdateVariables(Time* t);

	// 资源路径
	std::string resourcePath;

	// Mod管理
	static EventFactory* eventFactory;
	static ChangeFactory* changeFactory;

	Script* script;
	std::unordered_map<std::string, ValueType> variables;
};

