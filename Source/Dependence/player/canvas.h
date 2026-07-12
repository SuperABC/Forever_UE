#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>


struct IntRingBuf {
	static const int CAP = 128;
	int data[CAP];
	int head, tail, cnt; // 队头、队尾、元素数量
	bool empty() const { return cnt == 0; }
	void push(int v) { if (cnt < CAP) { data[tail] = v; tail = (tail + 1) % CAP; ++cnt; } }
	int front() const { return data[head]; }
	void pop() { if (cnt > 0) { head = (head + 1) % CAP; --cnt; } }
	void clear() { head = 0; tail = 0; cnt = 0; }
};

struct MouseState {
	int x, y;                        // 鼠标当前坐标
	bool buttons[3]; // 0=左键 1=右键 2=中键
};

class Canvas {
public:
	Canvas();

	/*
	* 初始化画布，分配像素缓冲区并填充黑色背景
	* @width: 画布宽度（像素）
	* @height: 画布高度（像素）
	*/
	void Init(int width, int height);

	/*
	* 获取画布宽度
	* @return: 画布宽度（像素）
	*/
	int GetWidth() const;

	/*
	* 获取画布高度
	* @return: 画布高度（像素）
	*/
	int GetHeight() const;

	/*
	* 获取像素缓冲区原始数据指针（BGRA格式）
	* @return: 指向缓冲区首字节的只读指针
	*/
	const uint8_t* GetData() const;

	/*
	* 获取像素缓冲区字节数
	* @return: 缓冲区总字节数
	*/
	int GetDataSize() const;

	/*
	* 设置当前画笔颜色（RGB）
	* @r: 红色分量
	* @g: 绿色分量
	* @b: 蓝色分量
	*/
	void SetColor(uint8_t r, uint8_t g, uint8_t b);

	/*
	* 设置当前画笔透明度
	* @a: Alpha值（255=完全不透明）
	*/
	void SetAlpha(uint8_t a);

	/*
	* 设置字体大小
	* @size: 字号（像素高度）
	*/
	void SetFontSize(int size);

	/*
	* 设置字体名称（UTF-8编码）
	* @name: 字体名称字符串
	*/
	void SetFontName(const std::string& name);

	/*
	* 设置字体样式
	* @bold: 是否粗体
	* @italic: 是否斜体
	* @underline: 是否下划线
	*/
	void SetFontStyle(bool bold, bool italic, bool underline);

	/*
	* 获取当前字体名称
	* @return: 字体名称字符串引用
	*/
	const std::string& GetFontName() const;

	/*
	* 获取当前字体大小
	* @return: 字号（像素高度）
	*/
	int GetFontSize() const;

	/*
	* 查询当前字体是否粗体
	* @return: 粗体为true，否则false
	*/
	bool IsFontBold() const;

	/*
	* 查询当前字体是否斜体
	* @return: 斜体为true，否则false
	*/
	bool IsFontItalic() const;

	/*
	* 查询当前字体是否有下划线
	* @return: 有下划线为true，否则false
	*/
	bool IsFontUnderline() const;

	/*
	* 清空画布缓冲区，填充为黑色不透明背景
	*/
	void ClearBuffer();

	/*
	* 在指定坐标绘制单个像素（使用当前画笔颜色和透明度）
	* @x: 横坐标
	* @y: 纵坐标
	*/
	void PutPixel(int x, int y);

	/*
	* 读取指定坐标的像素颜色值
	* @x: 横坐标
	* @y: 纵坐标
	* @r: 输出红色分量
	* @g: 输出绿色分量
	* @b: 输出蓝色分量
	* @a: 输出Alpha分量
	*/
	void GetPixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;

	/*
	* 绘制直线（Bresenham算法）
	* @x1: 起点横坐标
	* @y1: 起点纵坐标
	* @x2: 终点横坐标
	* @y2: 终点纵坐标
	*/
	void PutLine(int x1, int y1, int x2, int y2);

	/*
	* 绘制矩形（空心或填充）
	* @x1: 左上角横坐标
	* @y1: 左上角纵坐标
	* @x2: 右下角横坐标
	* @y2: 右下角纵坐标
	* @fill: true为填充，false为空心
	*/
	void PutRect(int x1, int y1, int x2, int y2, bool fill);

	/*
	* 绘制三角形（空心或填充，重心坐标法）
	* @x1: 顶点1横坐标
	* @y1: 顶点1纵坐标
	* @x2: 顶点2横坐标
	* @y2: 顶点2纵坐标
	* @x3: 顶点3横坐标
	* @y3: 顶点3纵坐标
	* @fill: true为填充，false为空心
	*/
	void PutTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill);

	/*
	* 绘制圆形（空心或填充）
	* @x: 圆心横坐标
	* @y: 圆心纵坐标
	* @radius: 半径（像素）
	* @fill: true为填充，false为空心
	*/
	void PutCircle(int x, int y, int radius, bool fill);

	/*
	* 绘制椭圆（空心或填充）
	* @x: 圆心横坐标
	* @y: 圆心纵坐标
	* @radiusX: 水平半径（像素）
	* @radiusY: 垂直半径（像素）
	* @fill: true为填充，false为空心
	*/
	void PutEllipse(int x, int y, int radiusX, int radiusY, bool fill);

	/*
	* 绘制文字（使用 Windows GDI + 当前字体设置）
	* @text: 要绘制的文本（UTF-8编码）
	* @x: 文字左上角横坐标
	* @y: 文字左上角纵坐标
	*/
	void PutString(const std::string& text, int x, int y);

	/*
	* 绘制原始BGRA图像数据（缩放到dstW x dstH）
	* @srcBGRA: 源图像数据指针（BGRA格式）
	* @srcW: 源图像宽度
	* @srcH: 源图像高度
	* @x: 目标左上角横坐标
	* @y: 目标左上角纵坐标
	* @dstW: 目标绘制宽度
	* @dstH: 目标绘制高度
	*/
	void PutRawImage(const uint8_t* srcBGRA, int srcW, int srcH, int x, int y, int dstW, int dstH);

	/*
	* 推送键盘事件（高位0x8000=0表示按下，=1表示抬起）
	* @key: 键码值
	*/
	void PushKey(int key);

	/*
	* 查询或弹出键盘事件队列
	* @cmd: 0=弹出并返回队首键值（空则返回0），1=队列非空返回1否则0
	* @return: 键码值或队列状态
	*/
	int BiosKey(int cmd);

	/*
	* 清空键盘事件队列
	*/
	void ClearKeyBuffer();

	/*
	* 注册键名别名，由宿主层（CanvasBuffer）在初始化时填充
	* @name: 键名字符串
	* @code: 对应的键码值
	*/
	void RegisterKey(const char* name, int code);

	/*
	* 按键名查询键码，供纯C++侧比较
	* @name: 键名字符串
	* @return: 键码值，未注册返回0
	*/
	int KeyCode(const char* name) const;

	/*
	* 更新鼠标位置（不入队）
	* @x: 横坐标
	* @y: 纵坐标
	*/
	void SetMousePos(int x, int y);

	/*
	* 更新鼠标按键状态并推送点击事件
	* @button: 按键索引（0=左键，1=右键，2=中键）
	* @down: true=按下，false=抬起
	*/
	void PushMouseButton(int button, bool down);

	/*
	* 返回鼠标实时状态快照（位置+三键状态）
	* @return: 当前鼠标状态结构体
	*/
	MouseState MouseStatus() const;

	/*
	* 查询或弹出鼠标事件队列
	* @cmd: 0=弹出并返回队首点击事件（空则返回-1），1=队列非空返回1否则0
	* @return: 事件值或队列状态
	*/
	int BiosMouse(int cmd);

	/*
	* 清空鼠标事件队列
	*/
	void ClearMouseBuffer();

private:
	int width, height;                // 画布宽高（像素）
	std::vector<uint8_t> buffer;      // 像素缓冲区（BGRA格式）

	uint8_t brushR, brushG, brushB, brushA; // 当前画笔颜色及透明度
	int fontSize;                     // 当前字体大小
	std::string fontName;             // 当前字体名称
	bool fontBold, fontItalic, fontUnderline; // 字体样式标志

	IntRingBuf keyQueue;              // 键盘事件环形缓冲队列
	std::unordered_map<std::string, int> keyAliases; // 键名到键码的映射表

	MouseState mouseState;            // 鼠标实时状态
	IntRingBuf mouseQueue;            // 鼠标事件环形缓冲队列

	/*
	* 绘制水平线段（内部辅助函数，用于填充图形）
	* @x1: 起始横坐标
	* @x2: 结束横坐标
	* @y: 纵坐标
	*/
	void FastLine(int x1, int x2, int y);
};
