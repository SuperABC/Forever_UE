#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>


struct IntRingBuf {
	static const int CAP = 128;
	int data[CAP];
	int head = 0, tail = 0, cnt = 0;
	bool empty() const { return cnt == 0; }
	void push(int v) { if (cnt < CAP) { data[tail] = v; tail = (tail + 1) % CAP; ++cnt; } }
	int front() const { return data[head]; }
	void pop() { if (cnt > 0) { head = (head + 1) % CAP; --cnt; } }
	void clear() { head = 0; tail = 0; cnt = 0; }
};

struct MouseState {
	int x = 0, y = 0;
	bool buttons[3] = { false, false, false }; // 0=左键 1=右键 2=中键
};

class Canvas {
public:
	Canvas();

	void Init(int width, int height);
	int GetWidth() const;
	int GetHeight() const;
	const uint8_t* GetData() const;
	int GetDataSize() const;

	void SetColor(uint8_t r, uint8_t g, uint8_t b);
	void SetAlpha(uint8_t a);
	void SetFontSize(int size);
	void SetFontName(const std::string& name);
	void SetFontStyle(bool bold, bool italic, bool underline);

	const std::string& GetFontName() const;
	int GetFontSize() const;
	bool IsFontBold() const;
	bool IsFontItalic() const;
	bool IsFontUnderline() const;

	void ClearBuffer();
	void PutPixel(int x, int y);
	void GetPixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;
	void PutLine(int x1, int y1, int x2, int y2);
	void PutRect(int x1, int y1, int x2, int y2, bool fill);
	void PutTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill);
	void PutCircle(int x, int y, int radius, bool fill);
	void PutEllipse(int x, int y, int radiusX, int radiusY, bool fill);

	// 绘制文字（使用 Windows GDI + 当前字体设置）
	void PutString(const std::string& text, int x, int y);

	// 绘制原始BGRA图像数据（缩放到dstW x dstH）
	void PutRawImage(const uint8_t* srcBGRA, int srcW, int srcH, int x, int y, int dstW, int dstH);

	// 推送键盘事件（0x8000=0 按下，0x8000=1 抬起）
	void PushKey(int key);

	// cmd=0: 弹出并返回队首键值（空则返回0）；cmd=1: 队列非空返回1否则0
	int BiosKey(int cmd);

	// 清空键盘事件队列
	void ClearKeyBuffer();

	// 注册键名别名，由宿主层（CanvasBuffer）在初始化时填充
	void RegisterKey(const char* name, int code);

	// 按键名查询键码，供纯C++侧比较（未注册返回0）
	int KeyCode(const char* name) const;

	// 更新鼠标位置（不入队）
	void SetMousePos(int x, int y);

	// 更新鼠标按键状态并推送点击事件（0x8000=0按下，0x8000=1抬起，低位=按键索引）
	void PushMouseButton(int button, bool down);

	// 返回鼠标实时状态快照（位置+三键状态）
	MouseState MouseStatus() const;

	// cmd=0: 弹出并返回队首点击事件（空则返回-1）；cmd=1: 队列非空返回1否则0
	int BiosMouse(int cmd);

	// 清空鼠标事件队列
	void ClearMouseBuffer();

private:
	int width = 0, height = 0;
	std::vector<uint8_t> buffer;

	uint8_t brushR = 255, brushG = 255, brushB = 255, brushA = 255;
	int fontSize = 16;
	std::string fontName;
	bool fontBold = false, fontItalic = false, fontUnderline = false;

	IntRingBuf keyQueue;
	std::unordered_map<std::string, int> keyAliases;

	MouseState mouseState;
	IntRingBuf mouseQueue;

	void FastLine(int x1, int x2, int y);
};
