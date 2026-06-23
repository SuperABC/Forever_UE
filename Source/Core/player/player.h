#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include "player/phone.h"


class Player {
public:
	// 构造玩家
	Player();

	// 析构玩家
	~Player();

	// 读取配置文件
	void LoadConfigs() const;

	// 读取小游戏模组
	void InitPuzzles(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 读取应用模组
	void InitApps(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	// 初始化工业
	void Init();

	// 释放空间
	void Destroy();

	// 时钟周期
	void Tick(float delta);

	// 应用剧情变化
	void ApplyChange(Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	// 获取时钟
	Time* GetTime();

	// 获取手机
	Phone* GetPhone() const;

	// 小游戏工厂
	static PuzzleFactory* puzzleFactory;

	// 应用工厂
	static AppFactory* appFactory;

private:
	// 时钟
	OBJECT_HOLDER Time* time;

	// 手机
	OBJECT_HOLDER Phone* phone;

};

