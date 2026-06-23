#pragma once

#include "../common/utility.h"
#include "../common/error.h"

#include <string>
#include <functional>
#include <unordered_map>

class Canvas;

class AppMod {
public:
	// 无构造
	AppMod();

	// 无析构
	virtual ~AppMod();

	// 统一类型定义
	static const char* GetId();
	virtual const char* GetType() const = 0;
	virtual const char* GetName() = 0;

	// 初始化
	static void Init();

	// 主循环
	static int Loop(Canvas* canvas, int ms);

	// 返回
	static void Back(Canvas* canvas);

	// 强制刷新
	static void Refresh(Canvas* canvas);
};

class AppFactory {
public:
	// 注册应用
	void RegisterApp(const std::string& id,
		std::function<void()> init, std::function<int(Canvas*, int)> loop,
		std::function<void(Canvas*)> back, std::function<void(Canvas*)> refresh,
		std::function<AppMod*()> creator, std::function<void(AppMod*)> deleter);

	// 清空注册
	void RemoveAll();

	// 创建应用
	AppMod* CreateApp(const std::string& id) const;

	// 检查是否注册
	bool CheckRegistered(const std::string& id) const;

	// 设置启用配置
	void SetConfig(const std::string& name, bool config);

	// 初始化指定类型应用
	void InitApp(const std::string& id) const;

	// 循环指定类型应用
	int LoopApp(const std::string& id, Canvas* canvas, int ms) const;

	// 返回指定类型应用
	void BackApp(const std::string& id, Canvas* canvas) const;

	// 强制刷新指定类型应用
	void RefreshApp(const std::string& id, Canvas* canvas) const;

	// 析构应用
	void DestroyApp(AppMod* appMod) const;

private:
	struct Registry {
		std::function<void()> init;
		std::function<int(Canvas*, int)> loop;
		std::function<void(Canvas*)> back;
		std::function<void(Canvas*)> refresh;
		std::function<AppMod*()> creator;
		std::function<void(AppMod*)> deleter;
	};

	std::unordered_map<std::string, Registry> registries;
	std::unordered_map<std::string, bool> configs;
};
