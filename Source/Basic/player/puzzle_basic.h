#pragma once

#include "player/puzzle_mod.h"


// 测试小游戏
class TestPuzzle : public PuzzleMod {
public:
	/*
	* 构造测试小游戏
	*/
	TestPuzzle();

	/*
	* 析构测试小游戏
	*/
	virtual ~TestPuzzle();

	/*
	* 测试小游戏静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 测试小游戏动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 测试小游戏实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 初始化小游戏
	* @width: 画布宽度, height: 画布高度
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
	// 总实例数量
	static int count;

	// 画布宽度
	static int canvasW;

	// 画布高度
	static int canvasH;

	// 矩形X坐标
	static float posX;

	// 矩形Y坐标
	static float posY;

	// X方向速度
	static float velX;

	// Y方向速度
	static float velY;

	// 是否暂停
	static bool paused;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;
};
