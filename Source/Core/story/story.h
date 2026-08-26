#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include <set>
#include <tuple>


class Story {
public:
	/*
	* 构造剧情
	*/
	Story();

	/*
	* 析构剧情
	*/
	~Story();

	/*
	* 读取配置文件
	*/
	void LoadConfigs() const;

	/*
	* 读取脚本模组
	* @modHandles, dlls: 模组句柄映射与动态库列表
	*/
	void InitScripts(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 初始化剧情
	* @map, populace, player: 地图、人口与玩家对象
	*/
	void Init(Map* map, Populace* populace, Player* player);

	/*
	* 释放空间
	*/
	void Destroy();

	/*
	* 时钟周期
	* @player: 玩家对象
	*/
	void Tick(Player* player);

	/*
	* 应用剧情变化
	* @change, getValues: 变化对象与值获取回调列表
	* @targetScript: 指定目标脚本（nullptr时应用到主线剧情）
	*/
	std::vector<Event*> ApplyChange(const Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		Script* targetScript = nullptr);

	/*
	* 获取主线剧情
	*/
	Script* GetScript() const;

	/*
	* 记录对话
	* @speaker, content: 发言者与内容
	*/
	void AddTalk(const std::string& speaker, const std::string& content);

	/*
	* 获取对话历史
	*/
	const std::vector<std::pair<std::string, std::string>>& GetHistory() const;

	/*
	* 创建或重置一个计时器（同名计时器已存在则覆盖），到时后只触发一次
	* @name: 计时器名称
	* @time: 目标时刻的时间部分；若比当前时间的时间部分晚，到达时间取当前日期+该时间，否则取当前日期次日+该时间
	* @category, label: 计时器所属脚本的类型（global/zone/building/citizen/elevator/vehicle）与实体名称（global不需要名称），用于到时后精确匹配脚本
	*/
	void CreateTimer(const std::string& name, const Time& time, const std::string& category, const std::string& label);

	/*
	* 移除一个计时器
	* @name: 计时器名称
	*/
	void RemoveTimer(const std::string& name);

	/*
	* 按到达时间从早到晚取出最多maxCount个已到时的计时器（取出的计时器会从计时器表中移除），不足maxCount个则只返回实际到时的数量
	* @now: 当前时间
	* @maxCount: 最多取出的数量
	*/
	std::vector<std::tuple<std::string, std::string, std::string>> PopExpiredTimers(const Time& now, int maxCount);

	/*
	* 添加一条待广播的全局消息，供App等外部调用方通过PostHandle写入
	* @message: 消息内容
	*/
	void AddMessage(const std::string& message);

	/*
	* 取出所有待处理的全局消息（取出后队列清空）
	*/
	std::vector<std::string> PopMessages();

	// 获取玩家当前房间
	Room* GetCurrentRoom(Map* map) const;

	/*
	* 为事件注入 local 变量并追加到 getValues，返回创建的 Script*（无 local 时返回 nullptr）
	* 调用方在不再需要时应 pop_back getValues 并 delete 返回值
	* @event: 触发事件
	* @getValues: 值获取回调列表（引用）
	*/
	static Script* CreateLocal(Event* event,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	// 脚本工厂
	static ScriptFactory* scriptFactory;

private:
	// 主线剧情
	OBJECT_HOLDER Script* script;

	// 对话历史
	std::vector<std::pair<std::string, std::string>> historyTalk;

	// 当前游戏时间（每次Tick更新，用于计时器到时判断）
	Time currentTime;

	// 计时器集合（到达时间, 名称, 所属脚本类型, 所属脚本实体名称），按到达时间有序，begin()即最先到时的计时器
	std::set<std::tuple<Time, std::string, std::string, std::string>> timerSet;

	// 待广播的全局消息队列
	std::vector<std::string> pendingMessages;

};

