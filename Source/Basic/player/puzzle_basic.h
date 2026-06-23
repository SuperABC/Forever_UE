#pragma once

#include "player/puzzle_mod.h"


// 测试小游戏：40x40矩形在画布中随机方向移动，碰边反弹
class TestPuzzle : public PuzzleMod {
public:
	TestPuzzle();
	virtual ~TestPuzzle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static void Init(int width, int height);
	static int Loop(Canvas* canvas, int ms);

private:
	static int count;
	static int canvasW, canvasH;
	static float posX, posY;
	static float velX, velY;
	static bool paused;

	int id;
	std::string name;
};
