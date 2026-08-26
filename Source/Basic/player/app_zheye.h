#pragma once

#include "player/app_mod.h"

#include <string>
#include <vector>


// 者也应用 - 论坛类App
class ZheyeApp : public AppMod {
public:
	/*
	* 构造者也应用
	*/
	ZheyeApp();

	/*
	* 析构者也应用
	*/
	virtual ~ZheyeApp();

	/*
	* 者也应用静态类型标识
	* @return: 静态类型标识
	*/
	static const char* GetId();

	/*
	* 者也应用动态类型标识
	* @return: 动态类型标识
	*/
	virtual const char* GetType() const override;

	/*
	* 者也应用实例唯一名称
	* @return: 实例唯一名称
	*/
	virtual const char* GetName() override;

	/*
	* 初始化应用
	* @post: 向Core发起查询的句柄
	*/
	static void Init(PostHandle* post);

	/*
	* 每帧更新逻辑
	* @canvas: 当前画布
	* @ms: 距上帧毫秒数
	* @post: 向Core发起查询的句柄
	* @return: 帧状态码，0继续运行，非0退出
	*/
	static int Loop(Canvas* canvas, int ms, PostHandle* post);

	/*
	* 返回键响应函数
	* @canvas: 当前画布
	* @post: 向Core发起查询的句柄
	*/
	static void Back(Canvas* canvas, PostHandle* post);

	/*
	* 强制刷新响应函数
	* @canvas: 当前画布
	* @post: 向Core发起查询的句柄
	*/
	static void Refresh(Canvas* canvas, PostHandle* post);

private:
	// 界面状态
	enum Screen {
		SectionList,
		PostDetail
	};

	// 回复
	struct Reply {
		std::string author;
		std::string content;
	};

	// 帖子
	struct Post {
		std::string title;
		std::string author;
		std::string content;
		std::vector<Reply> replies;
	};

	// 版块
	struct Section {
		std::string name;
		std::vector<Post> posts;
	};

	// 帖子行的屏幕矩形（供鼠标点击命中测试）
	struct RowRect {
		int x1, y1, x2, y2;
		int postIndex;
	};

	// 分区标签的屏幕矩形（供鼠标点击命中测试）
	struct TabRect {
		int x1, y1, x2, y2;
		int sectionIndex;
	};

	// --- 数据加载 ---

	/*
	* 从public/zheye/data.json一次性读取全部版块/帖子/回复数据
	* @return: 成功返回true
	*/
	static bool LoadData();

	/*
	* 打开指定帖子
	* @sectionIndex: 版块下标
	* @postIndex: 帖子下标
	*/
	static void OpenPost(int sectionIndex, int postIndex);

	// --- 输入处理 ---

	static void HandleListKeys(Canvas* canvas);
	static void HandleListMouse(Canvas* canvas);
	static void HandleTabMouse(Canvas* canvas);
	static void HandleDetailKeys(Canvas* canvas);

	// --- 渲染 ---

	static void RenderSectionList(Canvas* canvas);
	static void RenderTabBar(Canvas* canvas);
	static void RenderPostDetail(Canvas* canvas);
	static void RenderEmptyState(Canvas* canvas, const std::string& msg);

	// --- 工具 ---

	/*
	* 读取并解析一个json文件
	* @path: 文件绝对路径
	* @out: 解析结果输出
	* @return: 成功返回true
	*/
	static bool ReadJsonFile(const std::string& path, class JsonValue& out);

	/*
	* 用初始化存档(data.json)覆盖实时存档(runtime.json)，清除上一局残留的动态修改
	*/
	static void ResetRuntimeSave();

	// 数据根目录（Source/Resources/public/zheye），Init()中通过PostHandle解析
	static std::string publicPath;
	static bool available;

	// 本局游戏是否已经用初始化存档覆盖过实时存档；DLL静态存储期与游戏进程同生命周期，
	// 因此这个标记天然实现了"整局游戏只重置一次"，不受App被反复打开/关闭的影响
	static bool runtimeSaveReady;

	// 版块数据（Init()时从runtime.json一次性全部读入内存）
	static std::vector<Section> sections;
	static int currentSection;
	static int selectedPost;
	static int listScrollTop;

	// 分区标签栏的水平滚动偏移（像素），保证选中的分区完整显示在屏幕内
	static int tabScrollX;

	// 当前打开的帖子（PostDetail状态）
	static Screen screen;
	static int openSectionIndex;
	static int openPostIndex;
	static int replyScrollTop;

	// 上一次渲染详情页时，内容是否超出了屏幕可视区域（决定下滚键是否生效）
	static bool detailOverflowed;

	// 帖子列表行命中区域（每帧渲染列表时重建）
	static std::vector<RowRect> rowRects;

	// 分区标签命中区域（每帧渲染标签栏时重建）
	static std::vector<TabRect> tabRects;

	// 实例名称缓存（GetName()用）
	std::string name;
};
