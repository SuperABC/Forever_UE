#pragma once

#include "player/puzzle_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModPuzzlesFunc)(PuzzleFactory* factory);

// 小游戏实体
class Puzzle {
public:
	/*
	* 禁止默认构造
	*/
	Puzzle() = delete;

	/*
	* 构造小游戏实体
	* @factory: 小游戏工厂
	* @puzzle: 小游戏类型
	*/
	Puzzle(PuzzleFactory* factory, const std::string& puzzle);

	/*
	* 析构小游戏实体
	*/
	~Puzzle();

	/*
	* 小游戏动态类型标识
	* @return: 动态类型标识
	*/
	std::string GetType() const;

	/*
	* 小游戏实例唯一名称
	* @return: 实例唯一名称
	*/
	std::string GetName() const;

	/*
	* 初始化小游戏
	* @width: 画布宽度
	* @height: 画布高度
	*/
	void Init(int width, int height) const;

	/*
	* 每帧更新逻辑
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @return: 帧状态码，0继续运行，非0退出
	*/
	int Loop(Canvas* canvas, int ms) const;

private:
	// 模组对象
	OBJECT_HOLDER PuzzleMod* mod;

	// 工厂
	PuzzleFactory* factory;

	// 小游戏类型
	std::string type;

	// 小游戏名称
	std::string name;
};

// 空小游戏
class EmptyPuzzle : public PuzzleMod {
public:
	/*
	* 构造空小游戏
	*/
	EmptyPuzzle();
	
	/*
	* 析构空小游戏
	*/
	virtual ~EmptyPuzzle();

	/*
	* 空小游戏静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 空小游戏动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 空小游戏实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 初始化小游戏
	* @width: 画布宽度
	* @height: 画布高度
	*/
	static void Init(int width, int height);

	/*
	* 每帧更新逻辑
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @return: 帧状态码，0继续运行，非0退出
	*/
	static int Loop(Canvas* canvas, int ms);

private:
	// 空小游戏实例计数器
	static int count;

	// 实例唯一ID
	int id;

	// 实例名称
	std::string name;
};
