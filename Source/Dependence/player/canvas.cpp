#include "canvas.h"

#include <algorithm>
#include <cmath>
#define NOMINMAX
#include <windows.h>

#pragma comment(lib, "gdi32.lib")


using namespace std;

static constexpr float kPI = 3.14159265358979f;

Canvas::Canvas() {

}

void Canvas::Init(int w, int h) {
	width = w;
	height = h;

	const int pixelCount = w * h;
	buffer.resize(pixelCount * 4);

	uint32_t* data = reinterpret_cast<uint32_t*>(buffer.data());
	for (int i = 0; i < pixelCount; i++) {
		data[i] = 0xFF000000u;
	}
}

int Canvas::GetWidth() const {
	return width;
}

int Canvas::GetHeight() const {
	return height;
}

const uint8_t* Canvas::GetData() const {
	return buffer.data();
}

int Canvas::GetDataSize() const {
	return static_cast<int>(buffer.size());
}

void Canvas::SetColor(uint8_t r, uint8_t g, uint8_t b) {
	brushR = r; brushG = g; brushB = b;
}

void Canvas::SetAlpha(uint8_t a) {
	brushA = a;
}

void Canvas::SetFontSize(int size) {
	fontSize = size;
}

void Canvas::SetFontName(const string& name) {
	fontName = name;
}

void Canvas::SetFontStyle(bool bold, bool italic, bool underline) {
	fontBold = bold; fontItalic = italic; fontUnderline = underline;
}

const string& Canvas::GetFontName() const {
	return fontName;
}

int Canvas::GetFontSize() const {
	return fontSize;
}

bool Canvas::IsFontBold() const {
	return fontBold;
}

bool Canvas::IsFontItalic() const {
	return fontItalic;
}

bool Canvas::IsFontUnderline() const {
	return fontUnderline;
}

void Canvas::ClearBuffer() {
	uint32_t* data = reinterpret_cast<uint32_t*>(buffer.data());
	const int pixelCount = width * height;
	for (int i = 0; i < pixelCount; i++) {
		data[i] = 0xFF000000u;
	}
}

void Canvas::FastLine(int x1, int x2, int y) {
	if (y < 0 || y >= height) return;
	int ax = clamp(min(x1, x2), 0, width - 1);
	int bx = clamp(max(x1, x2), 0, width - 1);
	for (int x = ax; x <= bx; x++) {
		PutPixel(x, y);
	}
}

void Canvas::PutPixel(int x, int y) {
	if (x < 0 || x >= width || y < 0 || y >= height) return;
	int idx = (y * width + x) * 4;
	if (brushA == 255) {
		buffer[idx] = brushB;
		buffer[idx + 1] = brushG;
		buffer[idx + 2] = brushR;
		buffer[idx + 3] = 255;
	}
	else {
		uint8_t ia = 255 - brushA;
		buffer[idx] = static_cast<uint8_t>((buffer[idx] * ia + brushB * brushA) / 255);
		buffer[idx + 1] = static_cast<uint8_t>((buffer[idx + 1] * ia + brushG * brushA) / 255);
		buffer[idx + 2] = static_cast<uint8_t>((buffer[idx + 2] * ia + brushR * brushA) / 255);
		buffer[idx + 3] = max(buffer[idx + 3], brushA);
	}
}

void Canvas::GetPixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		r = g = b = a = 0; return;
	}
	int idx = (y * width + x) * 4;
	b = buffer[idx];
	g = buffer[idx + 1];
	r = buffer[idx + 2];
	a = buffer[idx + 3];
}

void Canvas::PutLine(int x1, int y1, int x2, int y2) {
	int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
	int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
	int err = dx + dy;
	while (true) {
		PutPixel(x1, y1);
		if (x1 == x2 && y1 == y2) break;
		int e2 = 2 * err;
		if (e2 >= dy) { err += dy; x1 += sx; }
		if (e2 <= dx) { err += dx; y1 += sy; }
	}
}

void Canvas::PutRect(int x1, int y1, int x2, int y2, bool fill) {
	if (fill) {
		for (int y = min(y1, y2); y <= max(y1, y2); y++)
			FastLine(x1, x2, y);
	}
	else {
		PutLine(x1, y1, x2, y1);
		PutLine(x2, y1, x2, y2);
		PutLine(x2, y2, x1, y2);
		PutLine(x1, y2, x1, y1);
	}
}

void Canvas::PutTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill) {
	if (!fill) {
		PutLine(x1, y1, x2, y2);
		PutLine(x2, y2, x3, y3);
		PutLine(x3, y3, x1, y1);
		return;
	}
	int minX = min({x1, x2, x3}), maxX = max({x1, x2, x3});
	int minY = min({y1, y2, y3}), maxY = max({y1, y2, y3});
	float denom = static_cast<float>((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
	if (abs(denom) < 0.5f) return;
	for (int py = minY; py <= maxY; py++) {
		for (int px = minX; px <= maxX; px++) {
			float w1 = ((y2 - y3) * (px - x3) + (x3 - x2) * (py - y3)) / denom;
			float w2 = ((y3 - y1) * (px - x3) + (x1 - x3) * (py - y3)) / denom;
			float w3 = 1.f - w1 - w2;
			if (w1 >= 0.f && w2 >= 0.f && w3 >= 0.f) PutPixel(px, py);
		}
	}
}

void Canvas::PutCircle(int cx, int cy, int radius, bool fill) {
	if (radius <= 0) { PutPixel(cx, cy); return; }
	if (fill) {
		for (int dy = -radius; dy <= radius; dy++) {
			int dx = static_cast<int>(sqrt(static_cast<float>(radius * radius - dy * dy)));
			FastLine(cx - dx, cx + dx, cy + dy);
		}
		return;
	}
	int x = 0, y = radius, d = 3 - 2 * radius;
	auto plot8 = [&](int ox, int oy) {
		PutPixel(cx + ox, cy + oy); PutPixel(cx - ox, cy + oy);
		PutPixel(cx + ox, cy - oy); PutPixel(cx - ox, cy - oy);
		PutPixel(cx + oy, cy + ox); PutPixel(cx - oy, cy + ox);
		PutPixel(cx + oy, cy - ox); PutPixel(cx - oy, cy - ox);
	};
	while (x <= y) {
		plot8(x, y);
		if (d < 0) d += 4 * x + 6;
		else { d += 4 * (x - y) + 10; y--; }
		x++;
	}
}

void Canvas::PutEllipse(int cx, int cy, int radiusX, int radiusY, bool fill) {
	if (radiusX <= 0 || radiusY <= 0) { PutPixel(cx, cy); return; }
	if (fill) {
		for (int dy = -radiusY; dy <= radiusY; dy++) {
			float t = max(0.f, 1.f - static_cast<float>(dy * dy) / static_cast<float>(radiusY * radiusY));
			int dx = static_cast<int>(radiusX * sqrt(t));
			FastLine(cx - dx, cx + dx, cy + dy);
		}
		return;
	}
	int steps = max(4, (radiusX + radiusY) * 4);
	for (int i = 0; i < steps; i++) {
		float a1 = static_cast<float>(i) / steps * 2.f * kPI;
		float a2 = static_cast<float>(i + 1) / steps * 2.f * kPI;
		PutLine(
			cx + static_cast<int>(radiusX * cos(a1)), cy + static_cast<int>(radiusY * sin(a1)),
			cx + static_cast<int>(radiusX * cos(a2)), cy + static_cast<int>(radiusY * sin(a2)));
	}
}

void Canvas::PutString(const string& text, int x, int y) {
	if (text.empty() || width <= 0 || height <= 0) return;

	// UTF-8 → wide
	int wlen = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
	if (wlen <= 1) return;
	vector<wchar_t> wtext(wlen);
	MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, wtext.data(), wlen);
	int charCount = wlen - 1;

	// font name → wide，默认微软雅黑
	wstring wfont;
	if (!fontName.empty()) {
		int fnlen = MultiByteToWideChar(CP_UTF8, 0, fontName.c_str(), -1, nullptr, 0);
		wfont.resize(fnlen);
		MultiByteToWideChar(CP_UTF8, 0, fontName.c_str(), -1, wfont.data(), fnlen);
	}
	else {
		wfont = L"Microsoft YaHei";
	}

	HDC screenDC = GetDC(nullptr);
	HDC memDC = CreateCompatibleDC(screenDC);

	HFONT hFont = CreateFontW(
		fontSize, 0, 0, 0,
		fontBold ? FW_BOLD : FW_THIN,
		fontItalic ? TRUE : FALSE,
		fontUnderline ? TRUE : FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, FF_MODERN,
		wfont.c_str());
	HFONT oldFont = (HFONT)SelectObject(memDC, hFont);

	// 量测文字尺寸
	SIZE sz;
	GetTextExtentPoint32W(memDC, wtext.data(), charCount, &sz);
	if (sz.cx <= 0 || sz.cy <= 0) {
		SelectObject(memDC, oldFont);
		DeleteObject(hFont);
		DeleteDC(memDC);
		ReleaseDC(nullptr, screenDC);
		return;
	}

	// 创建 24-bit 位图
	BITMAPINFOHEADER bi = {};
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = sz.cx;
	bi.biHeight = sz.cy;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;

	HBITMAP hbm = CreateCompatibleBitmap(screenDC, sz.cx, sz.cy);
	HBITMAP oldBm = (HBITMAP)SelectObject(memDC, hbm);

	// 黑底白字
	RECT rc = { 0, 0, sz.cx, sz.cy };
	FillRect(memDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
	SetTextColor(memDC, RGB(255, 255, 255));
	SetBkMode(memDC, TRANSPARENT);
	TextOutW(memDC, 0, 0, wtext.data(), charCount);

	// 读回像素（24-bit BGR，自底向上）
	int rowStride = ((sz.cx * 3 + 3) & ~3);
	vector<uint8_t> bits(static_cast<size_t>(rowStride) * sz.cy);
	GetDIBits(memDC, hbm, 0, sz.cy, bits.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	// 合成到 canvas buffer
	for (int row = 0; row < sz.cy; row++) {
		int srcRow = sz.cy - 1 - row;
		for (int col = 0; col < sz.cx; col++) {
			uint8_t gray = bits[static_cast<size_t>(srcRow) * rowStride + col * 3];
			if (!gray) continue;
			int px = x + col, py = y + row;
			if (px < 0 || px >= width || py < 0 || py >= height) continue;
			int idx = (py * width + px) * 4;
			uint8_t a = static_cast<uint8_t>(static_cast<int>(gray) * brushA / 255);
			uint8_t ia = 255 - a;
			buffer[idx] = static_cast<uint8_t>((buffer[idx] * ia + static_cast<int>(brushB) * a) / 255);
			buffer[idx + 1] = static_cast<uint8_t>((buffer[idx + 1] * ia + static_cast<int>(brushG) * a) / 255);
			buffer[idx + 2] = static_cast<uint8_t>((buffer[idx + 2] * ia + static_cast<int>(brushR) * a) / 255);
			buffer[idx + 3] = max(buffer[idx + 3], a);
		}
	}

	SelectObject(memDC, oldBm);
	SelectObject(memDC, oldFont);
	DeleteObject(hbm);
	DeleteObject(hFont);
	DeleteDC(memDC);
	ReleaseDC(nullptr, screenDC);
}

void Canvas::PutRawImage(const uint8_t* srcBGRA, int srcW, int srcH, int x, int y, int dstW, int dstH) {
	for (int dy = 0; dy < dstH; dy++) {
		for (int dx = 0; dx < dstW; dx++) {
			int dstX = x + dx, dstY = y + dy;
			if (dstX < 0 || dstX >= width || dstY < 0 || dstY >= height) continue;
			int sx = dx * srcW / dstW, sy = dy * srcH / dstH;
			int srcIdx = (sy * srcW + sx) * 4;
			int dstIdx = (dstY * width + dstX) * 4;
			buffer[dstIdx] = srcBGRA[srcIdx];
			buffer[dstIdx + 1] = srcBGRA[srcIdx + 1];
			buffer[dstIdx + 2] = srcBGRA[srcIdx + 2];
			buffer[dstIdx + 3] = srcBGRA[srcIdx + 3];
		}
	}
}

void Canvas::PushKey(int key) {
	keyQueue.push(key);
}

int Canvas::BiosKey(int cmd) {
	if (cmd == 0) {
		if (keyQueue.empty()) return 0;
		int key = keyQueue.front();
		keyQueue.pop();
		return key;
	}
	return keyQueue.empty() ? 0 : 1;
}

void Canvas::ClearKeyBuffer() {
	keyQueue.clear();
}

void Canvas::RegisterKey(const char* name, int code) {
	keyAliases[name] = code;
}

int Canvas::KeyCode(const char* name) const {
	auto it = keyAliases.find(name);
	return it != keyAliases.end() ? it->second : 0;
}

void Canvas::SetMousePos(int x, int y) {
	mouseState.x = x;
	mouseState.y = y;
}

void Canvas::PushMouseButton(int button, bool down) {
	if (button < 0 || button > 2) return;
	mouseState.buttons[button] = down;
	int event = button & ~0x8000;
	if (!down) event |= 0x8000;
	mouseQueue.push(event);
}

MouseState Canvas::MouseStatus() const {
	return mouseState;
}

int Canvas::BiosMouse(int cmd) {
	if (cmd == 0) {
		if (mouseQueue.empty()) return -1;
		int event = mouseQueue.front();
		mouseQueue.pop();
		return event;
	}
	return mouseQueue.empty() ? 0 : 1;
}

void Canvas::ClearMouseBuffer() {
	mouseQueue.clear();
}
