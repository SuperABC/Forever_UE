#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include "story/condition.h"

#include <string>


class Milestone {
public:

	/*
	* 构造里程碑
	* @name: 里程碑名称
	* @triggers: 触发事件列表
	* @visible: 是否对玩家可见
	* @keep: 持续条件（内部作为 drop 存储）
	* @description: 里程碑描述文本
	* @goal: 里程碑目标文本
	* @dialogs: 关联对话列表
	* @changes: 触发后的变更列表
	* @subsequences: 后续故事序列名称列表
	*/
	Milestone(std::string name, std::vector<Event*> triggers, bool visible, Condition keep, std::string description,
		std::string goal, std::vector<Dialog*> dialogs, std::vector<Change*> changes, std::vector<std::string> subsequences);

	/*
	* 析构里程碑
	*/
	~Milestone();

	/*
	* 获取触发事件列表
	* @return: 事件指针列表
	*/
	std::vector<Event*> GetTriggers() const;

	/*
	* 获取关联对话列表
	* @return: 对话指针列表
	*/
	std::vector<Dialog*> GetDialogs() const;

	/*
	* 获取变更列表
	* @return: 变更指针列表
	*/
	std::vector<Change*> GetChanges() const;

	/*
	* 判断给定事件是否匹配本里程碑的某个触发条件
	* @e: 待匹配的事件
	* @getValues: 取值回调列表，用于条件求值
	* @return: 匹配则返回 true，否则返回 false
	*/
	bool MatchTrigger(Event* e,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues) const;

	/*
	* 获取里程碑名称
	* @return: 里程碑名称字符串
	*/
	std::string GetName() const;

	/*
	* 判断里程碑是否对玩家可见
	* @return: 可见返回 true，否则返回 false
	*/
	bool IsVisible() const;

	/*
	* 获取丢弃条件对象
	* @return: 丢弃条件
	*/
	Condition DropCondition() const;

	/*
	* 根据当前值求值丢弃条件，判断是否应丢弃本里程碑
	* @getValues: 取值回调列表
	* @return: 应丢弃返回 true，否则返回 false
	*/
	bool DropSelf(const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues) const;

	/*
	* 获取里程碑描述文本
	* @return: 描述字符串
	*/
	std::string GetDescription() const;

	/*
	* 获取里程碑目标文本
	* @return: 目标字符串
	*/
	std::string GetGoal() const;

	/*
	* 获取后续故事序列名称列表
	* @return: 序列名称字符串列表
	*/
	std::vector<std::string> GetSubsequences() const;

private:

	// 里程碑名称
	std::string name;

	// 触发事件列表（持有所有权）
	OBJECT_HOLDER std::vector<Event*> triggers;

	// 是否对玩家可见
	bool visible;

	// 丢弃条件
	Condition drop;

	// 里程碑描述文本
	std::string description;

	// 里程碑目标文本
	std::string goal;

	// 关联对话列表（持有所有权）
	OBJECT_HOLDER std::vector<Dialog*> dialogs;

	// 关联变更列表（持有所有权）
	OBJECT_HOLDER std::vector<Change*> changes;

	// 后续故事序列名称列表
	std::vector<std::string> subsequences;
};

class MilestoneNode {
public:

	/*
	* 默认构造空里程碑节点
	*/
	MilestoneNode();

	/*
	* 构造持有指定里程碑的节点
	* @milestone: 里程碑指针
	*/
	MilestoneNode(Milestone* milestone);

	// 关联的里程碑内容
	Milestone* content;

	// 前置未满足数量
	int premise;

	// 后置里程碑节点列表
	std::vector<MilestoneNode *> subsequents;
};

