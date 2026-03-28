#pragma once

#include "script.h"


class Story {
public:
	// 构造剧情
	Story();

	// 析构剧情
	~Story();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取 Mods
	void InitScripts(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化剧情
	void Init();

	// 释放空间
	void Destroy();

	// 应用剧情变化
	void ApplyChange(Change* change, Story* story,
		std::vector < std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 获取主线剧情
	Script* GetScript() const;

private:
	// 统一工厂
	static ScriptFactory* scriptFactory;

	// 主线剧情
	OBJECT_HOLDER Script* script;

	// 剧情合集
	std::unordered_map<std::string, std::vector<Script*>> objectScripts;
};

