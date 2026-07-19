#pragma once
#include "player/puzzle_mod.h"


// 飞翔小鸟小游戏
class FlappyPuzzle : public PuzzleMod {
public:
	/*
	* 构造飞翔小鸟小游戏
	*/
	FlappyPuzzle();

	/*
	* 析构飞翔小鸟小游戏
	*/
	virtual ~FlappyPuzzle();

	/*
	* 飞翔小鸟小游戏静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 飞翔小鸟小游戏动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 飞翔小鸟小游戏实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 初始化小游戏
	* @width: 画布宽度, height: 画布高度
	* @post: 向Core发起查询的句柄
	*/
	static void Init(int width, int height, PostHandle* post);

	/*
	* 每帧更新逻辑
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @post: 向Core发起查询的句柄
	* @return: 帧状态码，0继续运行，非0退出
	*/
	static int Loop(Canvas* canvas, int ms, PostHandle* post);

private:
	// 最大管道数量
	static const int MAX_PIPES = 6;

	struct Pipe {
		float x, gapY, gapH, w;
		bool passed;
	};

	// 画布宽度
	static int canvasW;

	// 画布高度
	static int canvasH;

	// 小鸟X坐标
	static float birdX;

	// 小鸟Y坐标
	static float birdY;

	// 小鸟Y方向速度
	static float birdVY;

	// 小鸟碰撞半径
	static float birdR;

	// 悬停动画计时器
	static float hoverT;

	// 管道数组
	static Pipe pipes[MAX_PIPES];

	// 当前管道数量
	static int pipeCount;

	// 管道生成计时器
	static float pipeTimer;

	// 当前得分
	static int score;

	// 当前阶段（0=wait 1=play 2=dead）
	static int phase;

	// 重力加速度
	static float gravity;

	// 拍翅速度
	static float flapVel;

	// 管道移动速度
	static float pipeSpeed;

	// 管道生成间隔
	static float pipeInterval;

	// 管道缺口高度
	static float pipeGap;

	// 管道宽度
	static float pipeW;

	// 地面高度
	static float groundH;

	/*
	* 根据画布尺寸计算物理与布局参数
	*/
	static void ComputeLayout();

	/*
	* 在管道数组中生成一根新管道
	*/
	static void SpawnPipe();

	/*
	* 检测圆形与矩形是否碰撞
	* @cx: 圆心X, cy: 圆心Y, cr: 半径, rx1: 矩形左, ry1: 矩形上, rx2: 矩形右, ry2: 矩形下
	* @return: 是否碰撞
	*/
	static bool CircleRect(float cx, float cy, float cr,
						   float rx1, float ry1, float rx2, float ry2);

	/*
	* 检测小鸟与管道或地面是否发生碰撞
	* @return: 是否碰撞
	*/
	static bool Collides();

	/*
	* 在画布上绘制数字
	* @canvas: 当前画布, num: 数值, x: 起始X, y: 起始Y, scale: 缩放比例
	*/
	static void DrawNumber(Canvas* canvas, int num, int x, int y, int scale);

	/*
	* 绘制整帧场景
	* @canvas: 当前画布
	*/
	static void DrawScene(Canvas* canvas);
};
