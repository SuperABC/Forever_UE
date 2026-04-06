#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"


class Player {
public:
	// 构造玩家
	Player();

	// 析构玩家
	~Player();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取技能模组
	void InitSkills(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化工业
	void Init();

	// 释放空间
	void Destroy();

	// 时钟周期
	void Tick(float delta);

	// 应用剧情变化
	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>> getValues);

	// 获取时钟
	Time* GetTime();

	// 技能工厂
	static SkillFactory* skillFactory;

private:
	// 时钟
	OBJECT_HOLDER Time* time;

	// 全部技能
	OBJECT_HOLDER std::vector<Skill*> skills;
};

