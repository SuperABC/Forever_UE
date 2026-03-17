#pragma once

#include "../story/story.h"

#include "skill.h"
#include "utility.h"

#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>


class Player {
public:
	Player();
	~Player();

	// 读取Mods
	void InitSkills(std::unordered_map<std::string, HMODULE>& modHandles,
		std::vector<std::string>& dlls);

	// 读取配置文件
	void LoadConfigs() const;

	// 初始化全部交通
	void Init();

	// 释放空间
	void Destroy();

	// 时钟前进
	void Tick(int day, int hour, int min, int sec);

	// 输出当前交通
	void Print() const;

	// 保存/加载交通
	void Load(const std::string& path);
	void Save(const std::string& path) const;

	// 应用变更
	void ApplyChange(Change* change, Story* story,
		std::vector<std::function<std::pair<bool, ValueType>(
			const std::string&)>>&getValues);

	// 获取时钟
	Time* GetTime();

private:
	// Mod管理
	static SkillFactory* skillFactory;

	// 时钟
	Time* time;

	std::vector<Skill*> skills;
};

