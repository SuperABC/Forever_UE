#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>
#include <unordered_map>

class Canvas;

// 小游戏Mod基类
class PuzzleMod {
public:
	/*
	* 构造小游戏
	*/
	PuzzleMod();

	/*
	* 析构小游戏
	*/
	virtual ~PuzzleMod();

	/*
	* Override
	* 小游戏静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 小游戏动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 小游戏实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 初始化小游戏
	* @width: 画布宽度, height: 画布高度
	*/
	static void Init(int width, int height);

	/*
	* Override
	* 每帧更新逻辑
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @return: 帧状态码，0继续运行，非0退出
	*/
	static int Loop(Canvas* canvas, int ms);
};

class PuzzleFactory {
public:
	/*
	* 注册小游戏
	* @id: 小游戏类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterPuzzle(const std::string& id,
		std::function<void(int, int)> init, std::function<int(Canvas*, int)> loop,
		std::function<PuzzleMod* ()> creator, std::function<void(PuzzleMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建小游戏
	* @id: 小游戏类型
	*/
	PuzzleMod* CreatePuzzle(const std::string& id) const;

	/*
	* 检查小游戏是否已注册
	* @id: 小游戏类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置小游戏启用配置
	* @name: 小游戏类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 初始化小游戏
	* @id: 小游戏类型
	* @width: 画布宽度, height: 画布高度
	*/
	void InitPuzzle(const std::string& id, int width, int height) const;

	/*
	* 每帧更新小游戏
	* @id: 小游戏类型
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @return: 帧状态码，0继续运行，非0退出
	*/
	int LoopPuzzle(const std::string& id, Canvas* canvas, int ms) const;

	/*
	* 析构小游戏
	* @puzzleMod: 待析构的小游戏
	*/
	void DestroyPuzzle(PuzzleMod* puzzleMod) const;

private:
	struct Registry {
		std::function<void(int, int)> init;
		std::function<int(Canvas*, int)> loop;
		std::function<PuzzleMod* ()> creator;
		std::function<void(PuzzleMod*)> deleter;
	};

	// 注册表
	std::unordered_map<std::string, Registry> registries;

	// 启用配置
	std::unordered_map<std::string, bool> configs;
};
