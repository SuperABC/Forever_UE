#include "puzzle_basic.h"
#include "player/canvas.h"

#include <cstdlib>
#include <cmath>


using namespace std;

static const int RECT_SIZE = 40;
static const float SPEED = 150.0f;

int TestPuzzle::count = 0;
int TestPuzzle::canvasW = 0;
int TestPuzzle::canvasH = 0;
float TestPuzzle::posX = 0.0f;
float TestPuzzle::posY = 0.0f;
float TestPuzzle::velX = 0.0f;
float TestPuzzle::velY = 0.0f;
bool TestPuzzle::paused = false;

TestPuzzle::TestPuzzle() : id(count++) {

}

TestPuzzle::~TestPuzzle() {

}

const char* TestPuzzle::GetId() {
	return "test";
}

const char* TestPuzzle::GetType() const {
	return "test";
}

const char* TestPuzzle::GetName() {
	name = "测试小游戏" + to_string(id);
	return name.data();
}

void TestPuzzle::Init(int width, int height, PostHandle* post) {
	canvasW = width;
	canvasH = height;

	posX = static_cast<float>(width - RECT_SIZE) / 2.0f;
	posY = static_cast<float>(height - RECT_SIZE) / 2.0f;

	// 随机角度在 [15°, 75°]，再随机翻转象限，避免纯水平/垂直运动
	float angle = (15.0f + static_cast<float>(rand() % 61)) * 3.14159265f / 180.0f;
	velX = SPEED * cosf(angle) * (rand() % 2 ? 1.0f : -1.0f);
	velY = SPEED * sinf(angle) * (rand() % 2 ? 1.0f : -1.0f);
}

int TestPuzzle::Loop(Canvas* canvas, int ms, PostHandle* post) {
	if (!canvas || canvasW <= 0 || canvasH <= 0) return 0;

	int key;
	while ((key = canvas->BiosKey(0)) != 0) {
		if ((key & 0x8000) == 0 && (key & ~0x8000) == canvas->KeyCode("space")) {
			paused = !paused;
		}
	}

	int mev;
	while ((mev = canvas->BiosMouse(0)) != -1) {
		if ((mev & 0x8000) == 0 && (mev & ~0x8000) == 0) {
			MouseState mouse = canvas->MouseStatus();
			posX = static_cast<float>(mouse.x - RECT_SIZE / 2);
			posY = static_cast<float>(mouse.y - RECT_SIZE / 2);
		}
	}

	if (!paused) {
		float dt = ms / 1000.0f;
		posX += velX * dt;
		posY += velY * dt;

		if (posX < 0.0f) {
			posX = 0.0f;
			velX = fabsf(velX);
		} else if (posX + RECT_SIZE > canvasW) {
			posX = static_cast<float>(canvasW - RECT_SIZE);
			velX = -fabsf(velX);
		}

		if (posY < 0.0f) {
			posY = 0.0f;
			velY = fabsf(velY);
		} else if (posY + RECT_SIZE > canvasH) {
			posY = static_cast<float>(canvasH - RECT_SIZE);
			velY = -fabsf(velY);
		}
	}

	canvas->ClearBuffer();
	canvas->SetColor(255, 255, 255);
	canvas->SetAlpha(255);
	canvas->PutRect(static_cast<int>(posX), static_cast<int>(posY),
		static_cast<int>(posX) + RECT_SIZE - 1, static_cast<int>(posY) + RECT_SIZE - 1, true);
	return 0;
}
