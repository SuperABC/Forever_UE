#pragma once

#include "story/script_mod.h"


// 默认NPC脚本
class NPCScript : public ScriptMod {
public:
	/*
	* 构造默认NPC脚本
	*/
	NPCScript();

	/*
	* 析构默认NPC脚本
	*/
	virtual ~NPCScript();

	/*
	* 默认NPC脚本静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 默认NPC脚本动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 默认NPC脚本实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 标记是否为主线剧情
	* @return: 是否为主线
	*/
	static bool MainStory();

	/*
	* 设置脚本
	*/
	virtual void SetScript();

	/*
	* Override
	* 修正剧情
	* @event: 关联事件
	* @getValues: 变量取值函数列表
	* @post: 向Core发起查询的句柄
	*/
	virtual void WrapScript(const Event* event,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		PostHandle* post) override;

private:
	// 全局实例计数
	static int count;

	// 实例唯一编号
	int id;

	// 剧本名称
	std::string name;
};

// 默认电梯脚本
class ElevatorScript : public ScriptMod {
public:
	/*
	* 构造默认电梯脚本
	*/
	ElevatorScript();

	/*
	* 析构默认电梯脚本
	*/
	virtual ~ElevatorScript();

	/*
	* 默认电梯脚本静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 默认电梯脚本动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 默认电梯脚本实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 标记是否为主线剧情
	* @return: 是否为主线
	*/
	static bool MainStory();

	/*
	* 设置脚本
	*/
	virtual void SetScript();

	/*
	* Override
	* 修正剧情
	* @event: 关联事件
	* @getValues: 变量取值函数列表
	* @post: 向Core发起查询的句柄
	*/
	virtual void WrapScript(const Event* event,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		PostHandle* post) override;

private:
	// 全局实例计数
	static int count;

	// 实例唯一编号
	int id;

	// 剧本名称
	std::string name;
};
