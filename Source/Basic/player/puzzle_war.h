#pragma once
#include "player/puzzle_mod.h"


// 战争小游戏
class WarPuzzle : public PuzzleMod {
public:
	/*
	* 构造战争小游戏
	*/
	WarPuzzle();

	/*
	* 析构战争小游戏
	*/
	virtual ~WarPuzzle();

	/*
	* 战争小游戏静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 战争小游戏动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 战争小游戏实例唯一名称
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
	// 棋盘边长（格数）
	static const int BOARD = 8;

	// 船只总数
	static const int SHIP_COUNT = 6;

	// 单艘船最大格数
	static const int MAX_CELLS = 6;

	// {rows, cols} for each of the 6 ships
	static const int SHAPES[SHIP_COUNT][2];

	struct ShipState {
		int cells[MAX_CELLS][2]; // absolute {row,col} on board
		int cellCount;
		bool sunk;
	};

	struct BoardState {
		int grid[BOARD][BOARD]; // -1=empty, >=0=ship index
		bool hit[BOARD][BOARD];
		ShipState ships[SHIP_COUNT];
	};

	// 画布宽高与单元格像素尺寸
	static int canvasW, canvasH, cellSize;

	// 玩家棋盘左上角坐标
	static int playerBoardX, playerBoardY;

	// AI棋盘左上角坐标
	static int aiBoardX, aiBoardY;

	// 玩家棋盘和AI棋盘状态
	static BoardState playerBoard, aiBoard;

	// 当前正在放置的船只索引
	static int placeIdx;

	// 放置光标位置与旋转状态
	static int placeCurR, placeCurC, placeRot;

	// 战斗阶段光标位置
	static int cursorR, cursorC;

	// 是否为玩家回合
	static bool playerTurn;

	// 游戏阶段：0=放置 1=战斗 2=结束
	static int phase;

	// 玩家是否获胜
	static bool playerWon;

	// AI行动延迟计时
	static float aiDelay;

	// 玩家总射击次数
	static int playerShots;

	/*
	* Tool
	* 计算棋盘布局坐标
	*/
	static void ComputeLayout();

	/*
	* 获取指定船只在特定旋转和位置下占据的格子坐标
	* @si: 船只索引
	* @rot: 旋转方向
	* @ar, ac: 锚点行列
	* @out: 输出格子坐标数组
	* @cnt: 输出格子数量
	*/
	static void GetCells(int si, int rot, int ar, int ac, int out[][2], int& cnt);

	/*
	* 检查船只是否可放置于指定位置
	* @b: 棋盘状态
	* @si: 船只索引
	* @rot: 旋转方向
	* @ar, ac: 锚点行列
	* @return: 是否可放置
	*/
	static bool CanPlace(BoardState& b, int si, int rot, int ar, int ac);

	/*
	* 将船只放置到棋盘上
	* @b: 棋盘状态
	* @si: 船只索引
	* @rot: 旋转方向
	* @ar, ac: 锚点行列
	*/
	static void PlaceShip(BoardState& b, int si, int rot, int ar, int ac);

	/*
	* AI自动放置所有船只
	*/
	static void AIPlace();

	/*
	* 向棋盘指定位置射击
	* @b: 棋盘状态
	* @r, c: 目标行列
	* @return: 是否命中
	*/
	static bool FireAt(BoardState& b, int r, int c);

	/*
	* 检查指定船只是否已被击沉
	* @b: 棋盘状态
	* @si: 船只索引
	*/
	static void CheckSunk(BoardState& b, int si);

	/*
	* 检查棋盘上所有船只是否全部沉没
	* @b: 棋盘状态
	* @return: 是否全部沉没
	*/
	static bool AllSunk(const BoardState& b);

	/*
	* AI执行射击行动
	*/
	static void AIFire();

	/*
	* 绘制棋盘到画布
	* @canvas: 当前画布
	* @b: 棋盘状态
	* @bx, by: 棋盘左上角坐标
	* @hideShips: 是否隐藏船只（用于AI棋盘）
	* @showCursor: 是否显示光标
	* @cr, cc: 光标行列
	*/
	static void DrawBoard(Canvas* canvas, const BoardState& b, int bx, int by,
						  bool hideShips, bool showCursor, int cr, int cc);

	/*
	* 绘制船只放置预览幽灵
	* @canvas: 当前画布
	* @bx, by: 棋盘左上角坐标
	* @si: 船只索引
	* @rot: 旋转方向
	* @ar, ac: 锚点行列
	*/
	static void DrawGhost(Canvas* canvas, int bx, int by, int si, int rot, int ar, int ac);

	/*
	* 绘制待放置船只列表
	* @canvas: 当前画布
	* @x, y: 列表起始坐标
	* @availH: 可用高度
	*/
	static void DrawShipList(Canvas* canvas, int x, int y, int availH);

	/*
	* 绘制数字到画布
	* @canvas: 当前画布
	* @num: 要绘制的数字
	* @x, y: 绘制坐标
	* @scale: 缩放比例
	*/
	static void DrawNumber(Canvas* canvas, int num, int x, int y, int scale);

	/*
	* 绘制完整游戏场景
	* @canvas: 当前画布
	*/
	static void DrawScene(Canvas* canvas);
};
