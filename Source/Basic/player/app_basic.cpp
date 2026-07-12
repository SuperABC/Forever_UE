#include "app_basic.h"
#include "player/canvas.h"

#include <cstdlib>
#include <cmath>


using namespace std;

static const int APP_W = 480;
static const int APP_H = 640;
static const float SPEED = 120.0f;

int TestApp::count = 0;
float TestApp::posX = 0.0f;
float TestApp::posY = 0.0f;
float TestApp::velX = 0.0f;
float TestApp::velY = 0.0f;

TestApp::TestApp() : id(count++) {

}

TestApp::~TestApp() {

}

const char* TestApp::GetId() {
	return "test";
}

const char* TestApp::GetType() const {
	return "test";
}

const char* TestApp::GetName() {
	name = "测试应用" + to_string(id);
	return name.data();
}

void TestApp::Init() {
	posX = APP_W / 2.0f;
	posY = APP_H / 2.0f;

	float angle = static_cast<float>(rand() % 360) * 3.14159265f / 180.0f;
	velX = SPEED * cosf(angle);
	velY = SPEED * sinf(angle);
}

int TestApp::Loop(Canvas* canvas, int ms) {
	if (!canvas) return 0;

	float dt = ms / 1000.0f;
	posX += velX * dt;
	posY += velY * dt;

	if (posX < 0.0f) { posX = 0.0f; velX = fabsf(velX); }
	else if (posX >= APP_W) { posX = static_cast<float>(APP_W - 1); velX = -fabsf(velX); }

	if (posY < 0.0f) { posY = 0.0f; velY = fabsf(velY); }
	else if (posY >= APP_H) { posY = static_cast<float>(APP_H - 1); velY = -fabsf(velY); }

	canvas->ClearBuffer();
	canvas->SetColor(100, 200, 255);
	canvas->SetAlpha(255);
	canvas->PutRect(static_cast<int>(posX) - 5, static_cast<int>(posY) - 5, static_cast<int>(posX) + 5, static_cast<int>(posY) + 5, true);
	return 0;
}

void TestApp::Back(Canvas* canvas) {

}

void TestApp::Refresh(Canvas* canvas) {
	if (!canvas) return;
	canvas->ClearBuffer();
	canvas->SetColor(100, 200, 255);
	canvas->SetAlpha(255);
	canvas->PutRect(static_cast<int>(posX) - 5, static_cast<int>(posY) - 5, static_cast<int>(posX) + 5, static_cast<int>(posY) + 5, true);
}
