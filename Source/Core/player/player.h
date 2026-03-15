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

	// 设置资源路径
	void SetResourcePath(const std::string& path);

	// 读取Mods
	void InitSkills(std::unordered_map<std::string, HMODULE>& modHandles);

	// 初始化全部交通
	void Init();

	// 读取配置文件
	void ReadConfigs(const std::string& path) const;

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
	// 资源路径
	std::string resourcePath;

	// Mod管理
	static SkillFactory* skillFactory;

	// 时钟
	Time* time;

	std::vector<Skill*> skills;
};

