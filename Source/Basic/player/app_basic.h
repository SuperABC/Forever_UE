#pragma once

#include "player/app_mod.h"


// 测试应用
class TestApp : public AppMod {
public:
	/*
	* 构造测试应用
	*/
	TestApp();

	/*
	* 析构测试应用
	*/
	virtual ~TestApp();

	/*
	* 测试应用静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 测试应用动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 测试应用实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 初始化应用
	*/
	static void Init();

	/*
	* 每帧更新逻辑
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @return: 帧状态码，0继续运行，非0退出
	*/
	static int Loop(Canvas* canvas, int ms);

	/*
	* 返回键响应函数
	* @canvas: 当前画布
	*/
	static void Back(Canvas* canvas);

	/*
	* 强制刷新响应函数
	* @canvas: 当前画布
	*/
	static void Refresh(Canvas* canvas);

private:
	// 总实例数量
	static int count;

	// 小球X坐标
	static float posX;

	// 小球Y坐标
	static float posY;

	// X方向速度
	static float velX;

	// Y方向速度
	static float velY;

	// 实例编号
	int id;

	// 名称缓存
	std::string name;
};
