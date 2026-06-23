#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>
#include <unordered_map>

class Canvas;

class PuzzleMod {
public:
	// 无构造
	PuzzleMod();

	// 无析构
	virtual ~PuzzleMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 初始化
	static void Init(int width, int height);

	// 主循环
	static int Loop(Canvas* canvas, int ms);
};

class PuzzleFactory {
public:
	// 注册小游戏
	void RegisterPuzzle(const std::string& id,
		std::function<void(int, int)> init, std::function<int(Canvas*, int)> loop,
		std::function<PuzzleMod* ()> creator, std::function<void(PuzzleMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建小游戏
	PuzzleMod* CreatePuzzle(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 初始化指定类型小游戏
	void InitPuzzle(const std::string& id, int width, int height) const;

	// 循环指定类型小游戏
	int LoopPuzzle(const std::string& id, Canvas* canvas, int ms) const;

	// 析构小游戏实例
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
