#pragma once

#include "../common/utility.h"
#include "../common/error.h"
#include "../common/handle.h"

#include <string>
#include <functional>
#include <unordered_map>

class Canvas;

// 应用Mod基类
class AppMod {
public:
	/*
	* 构造应用
	*/
	AppMod();

	/*
	* 析构应用
	*/
	virtual ~AppMod();

	/*
	* Override
	* 应用静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* Override
	* 应用动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const = 0;

	/*
	* Override
	* 应用实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() = 0;

	/*
	* Override
	* 初始化应用
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME static void Init(PostHandle* post);

	/*
	* Override
	* 每帧更新逻辑
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @post: 向Core发起查询的句柄
	* @return: 帧状态码，0继续运行，非0退出
	*/
	COSTOM_RUNTIME static int Loop(Canvas* canvas, int ms, PostHandle* post);

	/*
	* Override
	* 返回键响应函数
	* @canvas: 当前画布
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME static void Back(Canvas* canvas, PostHandle* post);

	/*
	* Override
	* 强制刷新响应函数
	* @canvas: 当前画布
	* @post: 向Core发起查询的句柄
	*/
	COSTOM_RUNTIME static void Refresh(Canvas* canvas, PostHandle* post);
};

class AppFactory {
public:
	/*
	* 注册应用
	* @id: 应用类型
	* @creator, deleter: 构造与析构函数
	*/
	void RegisterApp(const std::string& id,
		std::function<void(PostHandle*)> init, std::function<int(Canvas*, int, PostHandle*)> loop,
		std::function<void(Canvas*, PostHandle*)> back, std::function<void(Canvas*, PostHandle*)> refresh,
		std::function<AppMod*()> creator, std::function<void(AppMod*)> deleter);

	/*
	* 清空所有注册
	*/
	void RemoveAll();

	/*
	* 创建应用
	* @id: 应用类型
	*/
	AppMod* CreateApp(const std::string& id) const;

	/*
	* 检查应用是否已注册
	* @id: 应用类型
	*/
	bool CheckRegistered(const std::string& id) const;

	/*
	* 设置应用启用配置
	* @name: 应用类型
	* @config: 是否启用
	*/
	void SetConfig(const std::string& name, bool config);

	/*
	* 初始化应用
	* @id: 应用类型
	* @post: 向Core发起查询的句柄
	*/
	void InitApp(const std::string& id, PostHandle* post) const;

	/*
	* 每帧更新小应用
	* @id: 应用类型
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @post: 向Core发起查询的句柄
	* @return: 帧状态码，0继续运行，非0退出
	*/
	int LoopApp(const std::string& id, Canvas* canvas, int ms, PostHandle* post) const;

	/*
	* 返回键响应函数
	* @canvas: 当前画布
	* @post: 向Core发起查询的句柄
	*/
	void BackApp(const std::string& id, Canvas* canvas, PostHandle* post) const;

	/*
	* 强制刷新响应函数
	* @canvas: 当前画布
	* @post: 向Core发起查询的句柄
	*/
	void RefreshApp(const std::string& id, Canvas* canvas, PostHandle* post) const;

	/*
	* 析构应用
	* @assetMod: 待析构的应用
	*/
	void DestroyApp(AppMod* appMod) const;

private:
	// 注册表
	struct Registry {
		std::function<void(PostHandle*)> init;
		std::function<int(Canvas*, int, PostHandle*)> loop;
		std::function<void(Canvas*, PostHandle*)> back;
		std::function<void(Canvas*, PostHandle*)> refresh;
		std::function<AppMod*()> creator;
		std::function<void(AppMod*)> deleter;
	};

	// 启用配置
	std::unordered_map<std::string, Registry> registries;
	std::unordered_map<std::string, bool> configs;
};
