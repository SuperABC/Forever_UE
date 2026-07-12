#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include "player/phone.h"


class Player {
public:
	/*
	* 构造玩家
	*/
	Player();

	/*
	* 析构玩家
	*/
	~Player();

	/*
	* 读取配置文件
	*/
	void LoadConfigs() const;

	/*
	* 读取并注册资产模组
	* @modHandles: 模组句柄表
	* @dlls: 模组路径列表
	*/
	void InitAssets(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取并注册小游戏模组
	* @modHandles: 模组句柄表
	* @dlls: 模组路径列表
	*/
	void InitPuzzles(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 读取并注册应用模组
	* @modHandles: 模组句柄表
	* @dlls: 模组路径列表
	*/
	void InitApps(std::unordered_map<std::string, HMODULE>& modHandles,
		const std::vector<std::string>& dlls);

	/*
	* 初始化玩家时钟与手机
	*/
	void Init();

	/*
	* 释放玩家持有的资源
	*/
	void Destroy();

	/*
	* 时钟周期更新
	* @delta: 本帧时长（秒）
	*/
	void Tick(float delta);

	/*
	* 应用剧情变化
	* @change: 变化事件
	* @getValues: 脚本变量获取函数集合
	*/
	void ApplyChange(Change* change,
		const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	/*
	* 获取游戏时钟
	*/
	Time* GetTime();

	/*
	* 是否发生跨天
	*/
	bool CrossDay();

	/*
	* 获取手机
	*/
	Phone* GetPhone() const;

	/*
	* 获取银行存款余额
	*/
	int GetDeposit() const;

	/*
	* 增减银行存款
	* @amount: 变化量（负值为减少）
	*/
	void AddDeposit(int amount);

	// 资产工厂
	static AssetFactory* assetFactory;

	// 小游戏工厂
	static PuzzleFactory* puzzleFactory;

	// 应用工厂
	static AppFactory* appFactory;

private:
	// 游戏时钟
	OBJECT_HOLDER Time* time;

	// 记录上一帧的游戏天数，用于跨天检测
	int day;

	// 手机
	OBJECT_HOLDER Phone* phone;

	// 银行存款余额
	int deposit;
};
