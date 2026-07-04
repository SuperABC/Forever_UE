#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/json.h"

#include "condition.h"

#include <string>
#include <queue>
#include <variant>
#include <functional>

class Event;
class Dialog;
class Change;

using ScriptAction = std::variant<Dialog*, Change*>;

class ScriptMod {
public:
	/*
	* 无构造
	*/
	ScriptMod();

	/*
	* 无析构
	*/
	virtual ~ScriptMod();

	/*
	* Override
	* 脚本静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 脚本动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 脚本实例名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 是否为主线剧情脚本
	*/
	COSTOM_INIT static bool MainStory();

	/*
	* Override
	* 设置脚本文件，将脚本路径写入scriptPath字段
	*/
	COSTOM_INIT virtual void SetScript() = 0;

	/*
	* Override
	* 脚本逻辑重载，修改actionQueue字段
	* @event: 触发事件
	* @getValues: 值获取回调列表
	*/
	COSTOM_RUNTIME virtual void WrapScript(const Event* event,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues) = 0;

	// 脚本文件路径
	std::string scriptPath;

	// 脚本动作队列（脚本前逻辑 + 脚本逻辑 + 脚本后逻辑）
	std::vector<ScriptAction> actionQueue;

};

class ScriptFactory {
public:
	/*
	* 注册脚本
	* @id: 脚本静态类型标识
	* @main: 是否为主线剧情脚本
	* @creator, deleter: 构造与析构方法
	*/
	void RegisterScript(const std::string& id, bool main,
		std::function<ScriptMod* ()> creator, std::function<void(ScriptMod*)> deleter);

	/*
	* 清空注册
	*/
	void RemoveAll();

	/*
	* 创建脚本
	* @id: 脚本静态类型标识
	*/
	ScriptMod* CreateScript(const std::string& id) const;

	/*
	* 检查是否注册
	* @id: 脚本静态类型标识
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置启用配置
	* @name: 脚本静态类型标识
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 获取主线剧情脚本类型标识
	*/
	std::string GetMain();

	/*
	* 析构脚本
	* @scriptMod: 待析构的脚本模组对象
	*/
	void DestroyScript(ScriptMod* scriptMod) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ScriptMod* ()>, std::function<void(ScriptMod*)>>
	> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 主线剧情脚本类型标识
	std::string main;

};

