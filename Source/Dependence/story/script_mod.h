#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/json.h"

#include "event.h"
#include "dialog.h"
#include "change.h"

#include <string>
#include <queue>
#include <variant>
#include <functional>


using Action = std::variant<Dialog*, Change*>;

class ScriptMod {
public:
	// 无构造
	ScriptMod();

	// 无析构
	virtual ~ScriptMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 是否为主线剧情
	COMSTOM_INIT static bool MainStory();

	// 指定脚本文件
	COMSTOM_INIT virtual void SetScript() = 0;

	// 脚本文件
	std::string scriptPath;

	// 脚本前逻辑
	COMSTOM_RUNTIME virtual void PreTrigger(const Event* event) = 0;
		
	// 脚本后逻辑
	COMSTOM_RUNTIME virtual void PostTrigger(const Event* event) = 0;
	
	// 脚本前逻辑+脚本逻辑+脚本后逻辑
	std::queue<Action> actionQueue;
};

class ScriptFactory {
public:
	// 注册脚本
	void RegisterScript(const std::string& id, bool main,
		std::function<ScriptMod* ()> creator, std::function<void(ScriptMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建脚本
	ScriptMod* CreateScript(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 获取主线剧情脚本
	std::string GetMain();

	// 析构脚本
	void DestroyScript(ScriptMod* terrain) const;

private:
	// 注册表
	std::unordered_map<
		std::string,
		std::pair<std::function<ScriptMod* ()>, std::function<void(ScriptMod*)>>
	> registries;
	
	// 启用配置
	std::unordered_map<std::string, bool> configs;

	// 主线剧情脚本
	std::string main;
};

