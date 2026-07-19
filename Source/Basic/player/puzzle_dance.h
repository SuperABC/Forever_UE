#pragma once
#include "player/puzzle_mod.h"


// 跳舞小游戏
class DancePuzzle : public PuzzleMod {
public:
	/*
	* 构造跳舞小游戏
	*/
	DancePuzzle();

	/*
	* 析构跳舞小游戏
	*/
	virtual ~DancePuzzle();

	/*
	* 跳舞小游戏静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 跳舞小游戏动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 跳舞小游戏实例唯一名称
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

	// 总轮次数
	static const int TOTAL_ROUNDS = 20;

	// 每轮最大箭头数
	static const int MAX_ARROWS = 8;

private:
	// 画布宽度
	static int canvasW;

	// 画布高度
	static int canvasH;

	// 当前轮次
	static int round;

	// 当前轮箭头数量
	static int arrowCount;

	// 箭头序列
	static int arrows[MAX_ARROWS];

	// 当前输入位置
	static int inputPos;

	// 输入是否完成
	static bool inputDone;

	// 各轮结果（0=fail 1=pass 3=perfect）
	static int results[TOTAL_ROUNDS];

	// 累计得分
	static int totalScore;

	// 当前轮已过时间
	static float elapsed;

	// 每轮持续时间
	static float roundDuration;

	// 当前阶段（0=input 1=result 2=done）
	static int phase;

	// 阶段计时器
	static float phaseTimer;

	// 是否触发错误闪烁
	static bool wrongFlash;

	// 错误闪烁计时器
	static float wrongFlashTimer;

	/*
	* 开始新一轮，生成箭头序列并重置状态
	*/
	static void StartRound();

	/*
	* 在画布上绘制单个箭头图标
	* @canvas: 当前画布, type: 箭头类型, cx: 中心X, cy: 中心Y, sz: 尺寸, r: 红, g: 绿, b: 蓝, a: 透明度
	*/
	static void DrawArrow(Canvas* canvas, int type, int cx, int cy, int sz,
						  uint8_t r, uint8_t g, uint8_t b, uint8_t a);

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

	/*
	* 计算指定轮次应生成的箭头数量
	* @rnd: 轮次编号
	* @return: 箭头数量
	*/
	static int ArrowCountForRound(int rnd);
};
