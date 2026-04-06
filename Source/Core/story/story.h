#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


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
		std::vector < std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 获取主线剧情
	Script* GetScript() const;

	// 脚本工厂
	static ScriptFactory* scriptFactory;

private:
	// 主线剧情
	OBJECT_HOLDER Script* script;
};

