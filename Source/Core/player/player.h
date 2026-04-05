#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Change;
class Player {
public:
	Player();
	~Player();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取 Mods
	void InitSkills(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化工业
	void Init(Map* map);

	// 释放空间
	void Destroy();

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 获取时钟
	Time* GetTime();

	// 统一工厂
	static SkillFactory* skillFactory;

private:
	// 时钟
	OBJECT_HOLDER Time* time;

	// 全部技能
	OBJECT_HOLDER std::vector<Skill*> skills;
};

