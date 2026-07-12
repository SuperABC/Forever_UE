#pragma once

#include "player/app_mod.h"

#include "class.h"


typedef void (*RegisterModAppsFunc)(AppFactory* factory);

// 应用实体
class App {
public:
	/*
	* 禁止默认构造
	*/
	App() = delete;

	/*
	* 构造App实体
	* @factory: 应用工厂
	* @app: 应用类型
	*/
	App(AppFactory* factory, const std::string& app);

	/*
	* 析构App实体
	*/
	~App();

	/*
	* 应用动态类型标识
	* @return: 动态类型标识
	*/
	std::string GetType() const;

	/*
	* 应用实例唯一名称
	* @return: 实例唯一名称
	*/
	std::string GetName() const;

	/*
	* 初始化应用
	*/
	void Init() const;

	/*
	* 每帧更新逻辑
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @return: 帧状态码，0继续运行，非0退出
	*/
	int Loop(Canvas* canvas, int ms) const;

	/*
	* 返回键响应函数
	* @canvas: 当前画布
	*/
	void Back(Canvas* canvas) const;

	/*
	* 强制刷新响应函数
	* @canvas: 当前画布
	*/
	void Refresh(Canvas* canvas) const;

private:
	// 应用模组对象
	OBJECT_HOLDER AppMod* mod;

	// 应用工厂指针
	AppFactory* factory;

	// 应用类型标识
	std::string type;

	// 应用名称
	std::string name;
};

class EmptyApp : public AppMod {
public:
	/*
	* 构造空应用
	*/
	EmptyApp();
	
	/*
	* 析构空应用
	*/
	virtual ~EmptyApp();

	/*
	* 空应用静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 空应用动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 空应用实例唯一名称
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
	// 空应用实例计数器
	static int count;

	// 实例唯一ID
	int id;

	// 实例名称
	std::string name;
};
