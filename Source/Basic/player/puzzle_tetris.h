#pragma once
#include "player/puzzle_mod.h"


// 俄罗斯方块小游戏
class TetrisPuzzle : public PuzzleMod {
public:
	/*
	* 构造俄罗斯方块小游戏
	*/
	TetrisPuzzle();

	/*
	* 析构俄罗斯方块小游戏
	*/
	virtual ~TetrisPuzzle();

	/*
	* 俄罗斯方块小游戏
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 俄罗斯方块小游戏
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 俄罗斯方块小游戏
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
	// 棋盘列数
	static const int COLS = 10;

	// 棋盘行数
	static const int ROWS = 20;

	// 单元格像素尺寸（由Init()根据画布大小计算）
	static int cellSize;

	// 棋盘左上角X坐标
	static int boardX;

	// 棋盘左上角Y坐标
	static int boardY;

	// 画布宽度
	static int canvasW;

	// 画布高度
	static int canvasH;

	// 数字显示缩放比例
	static int digitScale;

	// 棋盘格子状态（0=空，其余=方块类型）
	static int board[ROWS][COLS];

	// 当前方块类型
	static int currType;

	// 当前方块旋转状态
	static int currRot;

	// 当前方块X位置
	static int currX;

	// 当前方块Y位置
	static int currY;

	// 下一个方块类型
	static int nextType;

	// 当前得分
	static int score;

	// 当前等级
	static int level;

	// 已消除行数
	static int lines;

	// 重力下落计时器
	static float gravTimer;

	// 移动方向（-1/0/1）
	static int dasDir;

	// 延迟自动移动计时器
	static int dasTimer;

	// 是否游戏结束
	static bool gameOver;

	// 是否暂停
	static bool paused;

	// 是否刚消行
	static bool cleared;

	/*
	* 生成新的当前方块并随机选取下一个方块
	*/
	static void SpawnPiece();

	/*
	* 检测指定方块在给定位置是否可放置
	* @type: 方块类型, rot: 旋转状态, px: X位置, py: Y位置
	* @return: 是否可放置
	*/
	static bool CanPlace(int type, int rot, int px, int py);

	/*
	* 将当前方块锁定到棋盘
	*/
	static void LockPiece();

	/*
	* 消除已填满的行并返回消除数量
	* @return: 消除的行数
	*/
	static int ClearLines();

	/*
	* 计算当前方块的幽灵（预落点）Y坐标
	* @return: 幽灵Y坐标
	*/
	static int GhostY();

	/*
	* 绘制棋盘内容
	* @canvas: 当前画布
	*/
	static void DrawBoard(Canvas* canvas);

	/*
	* 绘制指定方块
	* @canvas: 当前画布, type: 方块类型, rot: 旋转状态, px: X位置, py: Y位置, alpha: 透明度
	*/
	static void DrawPiece(Canvas* canvas, int type, int rot, int px, int py, uint8_t alpha);

	/*
	* 绘制棋盘上的单个格子
	* @canvas: 当前画布, gx: 格子X, gy: 格子Y, type: 方块类型, alpha: 透明度
	*/
	static void DrawCell(Canvas* canvas, int gx, int gy, int type, uint8_t alpha);

	/*
	* 在画布上绘制数字
	* @canvas: 当前画布, num: 数值, x: 起始X, y: 起始Y, scale: 缩放比例
	*/
	static void DrawNumber(Canvas* canvas, int num, int x, int y, int scale);

	/*
	* 根据画布尺寸计算布局参数
	* @width: 画布宽度, height: 画布高度
	*/
	static void ComputeLayout(int width, int height);

	/*
	* 随机生成方块类型编号
	* @return: 方块类型编号
	*/
	static int RandType();

	/*
	* 根据当前等级计算重力下落间隔
	* @return: 下落间隔毫秒数
	*/
	static int GravInterval();
};
