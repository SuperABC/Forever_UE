#pragma once

#include "player/puzzle_mod.h"

#include "class.h"


// 子类注册函数
typedef void (*RegisterModPuzzlesFunc)(PuzzleFactory* factory);

// 小游戏实体
class Puzzle {
public:
	// 禁止默认构造
	Puzzle() = delete;

	// 通过类型从工厂构造
	Puzzle(PuzzleFactory* factory, const std::string& puzzle);

	// 析构小游戏
	~Puzzle();

	// 获取类型
	std::string GetType() const;

	// 获取名称
	std::string GetName() const;

	// 初始化棋盘
	void Init(int width, int height) const;

	// 推进游戏逻辑，返回0继续，非0结束
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
	EmptyPuzzle();
	virtual ~EmptyPuzzle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static void Init(int width, int height);
	static int Loop(Canvas* canvas, int ms);

private:
	static int count;

	int id;
	std::string name;
};
