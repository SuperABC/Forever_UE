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
	std::vector<Event*> ApplyChange(Change* change,
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

	/*
	* 添加登记资产，仅接受不动产和载具类别
	* @asset: 待登记的资产
	* @return: 是否添加成功，类别不符或已登记则返回 false
	*/
	bool AddSystemAsset(Asset* asset);

	/*
	* 转移登记资产所有权给调用方，从登记列表移除
	* @asset: 待转移的资产
	* @return: 资产指针，未登记则返回 nullptr
	*/
	Asset* RemoveSystemAsset(Asset* asset);

	/*
	* 获取所有登记资产
	* @return: 登记资产列表
	*/
	const std::vector<Asset*>& GetSystemAssets() const;

	/*
	* 按路径获取资产，路径格式为 root/容器1/.../asset名称，root 可为 left/right/back/room
	* @path: 资产路径，room 暂不支持，返回 nullptr
	* @return: 目标资产，路径无效或未找到则返回 nullptr
	*/
	Asset* GetByPath(const std::string& path);

	/*
	* 按路径移除资产并转移所有权给调用方，路径格式为 root/容器1/.../asset名称，root 可为 left/right/back/room
	* @path: 资产路径
	* @return: 被移除的资产，路径无效或未找到则返回 nullptr
	*/
	Asset* RemoveByPath(const std::string& path);

	/*
	* 按路径将资产添加到指定容器或槽位，路径格式为 root/容器1/.../asset名称，root 可为 left/right/back/room
	* @path: 容器路径
	* @asset: 待添加的资产
	* @return: 是否添加成功
	*/
	bool AddByPath(const std::string& path, Asset* asset);

	/*
	* 分割路径字符串，按 '/' 切分，忽略空段
	* @path: 待分割路径
	* @return: 路径各段列表
	*/
	static std::vector<std::string> SplitPath(const std::string& path);


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

	// 系统登记资产
	std::vector<Asset*> systemAsset;

	// 左手资产
	OBJECT_HOLDER Asset* leftHand;

	// 右手资产
	OBJECT_HOLDER Asset* rightHand;

	// 背包资产
	OBJECT_HOLDER Asset* backPack;
};
