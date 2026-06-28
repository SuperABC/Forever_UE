#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <unordered_map>


class Story {
public:
	// 构造剧情
	Story();

	// 析构剧情
	~Story();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取脚本模组
	void InitScripts(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化剧情
	void Init(Map* map, Populace* populace, Player* player);

	// 释放空间
	void Destroy();

	// 时钟周期
	void Tick(Player* player);

	// 应用剧情变化
	void ApplyChange(Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		Script* targetScript = nullptr);

	// 获取主线剧情
	Script* GetScript() const;

	// 记录对话
	void AddTalk(const std::string& speaker, const std::string& content);

	// 获取对话历史
	const std::vector<std::pair<std::string, std::string>>& GetHistory() const;

	/*
	* 创建或重置一个计时器（同名计时器已存在则覆盖），到时后只触发一次
	* @name: 计时器名称
	* @time: 目标时刻的时间部分；若比当前时间的时间部分晚，到达时间取当前日期+该时间，否则取当前日期次日+该时间
	*/
	void CreateTimer(const std::string& name, const Time& time);

	/*
	* 移除一个计时器
	* @name: 计时器名称
	*/
	void RemoveTimer(const std::string& name);

	// 获取所有计时器（名称 -> 到达时间）
	const std::unordered_map<std::string, Time>& GetTimers() const;

	// 脚本工厂
	static ScriptFactory* scriptFactory;

private:
	// 主线剧情
	OBJECT_HOLDER Script* script;

	// 对话历史
	std::vector<std::pair<std::string, std::string>> historyTalk;

	// 当前游戏时间（每次Tick更新，用于计时器到时判断）
	Time currentTime;

	// 计时器（名称 -> 到达时间）
	std::unordered_map<std::string, Time> timers;
};

