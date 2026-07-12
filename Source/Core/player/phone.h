#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include "player/app_mod.h"
#include "player/canvas.h"

#include <string>
#include <vector>


class Phone {
public:
	/*
	* 构造手机
	*/
	Phone();

	/*
	* 析构手机
	*/
	~Phone();

	/*
	* 初始化屏幕尺寸并重置状态
	* @width, height: 屏幕宽高（像素）
	*/
	void Init(int width, int height);

	/*
	* 驱动手机一帧（处理按键并渲染）
	* @canvas: 画布对象
	* @ms: 本帧时长（毫秒）
	*/
	int Loop(Canvas* canvas, int ms);

private:
	// 屏幕宽度
	int width;

	// 屏幕高度
	int height;

	// 底部工具栏Y坐标
	int barY;

	// 手机界面状态
	enum State { Home, InApp, TaskList };

	// 应用条目（类型、初始化标记、快照缓存）
	struct AppEntry {
		AppEntry(std::string type, bool initialized, std::vector<uint8_t> snapshot) :
			type(type),
			initialized(initialized),
			snapshot(snapshot) {
		}

		std::string type;

		bool initialized;

		std::vector<uint8_t> snapshot;
	};

	// 已安装的应用类型列表
	std::vector<std::string> appTypes;

	// 已打开的应用条目列表
	std::vector<AppEntry> entries;

	// 当前界面状态
	State state;

	// 桌面当前选中的应用序号
	int homeSelection;

	// 任务列表当前选中的条目序号
	int taskSelection;

	// 当前运行的应用类型
	std::string currentAppType;

	/*
	* 从工厂加载已启用的应用类型列表
	*/
	void BuildAppList();

	/*
	* 在条目列表中按类型查找索引（未找到返回-1）
	* @type: 应用类型标识
	*/
	int FindEntry(const std::string& type) const;

	/*
	* 打开或恢复指定应用
	* @canvas: 画布对象
	* @entryIdx: 条目索引
	*/
	void OpenApp(Canvas* canvas, int entryIdx);

	/*
	* 将当前画布内容保存为条目快照
	* @canvas: 画布对象
	* @entryIdx: 条目索引
	*/
	void SaveSnapshot(Canvas* canvas, int entryIdx);

	/*
	* 处理桌面状态下的按键输入
	* @canvas: 画布对象
	*/
	void HandleHomeKeys(Canvas* canvas);

	/*
	* 处理应用内状态下的按键输入
	* @canvas: 画布对象
	*/
	void HandleInAppKeys(Canvas* canvas);

	/*
	* 处理任务列表状态下的按键输入
	* @canvas: 画布对象
	*/
	void HandleTaskKeys(Canvas* canvas);

	/*
	* 渲染桌面界面
	* @canvas: 画布对象
	*/
	void RenderHome(Canvas* canvas);

	/*
	* 渲染任务列表界面
	* @canvas: 画布对象
	*/
	void RenderTaskList(Canvas* canvas);

	/*
	* 渲染底部工具栏
	* @canvas: 画布对象
	*/
	void RenderBottomBar(Canvas* canvas);
};
