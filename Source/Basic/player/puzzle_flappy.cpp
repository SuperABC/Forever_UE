#include "puzzle_flappy.h"
#include "player/canvas.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>


using namespace std;

static constexpr int PHASE_WAIT = 0;
static constexpr int PHASE_PLAY = 1;
static constexpr int PHASE_DEAD = 2;

static const uint8_t DIGIT_FONT[10][5] = {
	{0x7,0x5,0x5,0x5,0x7},{0x2,0x6,0x2,0x2,0x7},{0x7,0x1,0x7,0x4,0x7},
	{0x7,0x1,0x3,0x1,0x7},{0x5,0x5,0x7,0x1,0x1},{0x7,0x4,0x7,0x1,0x7},
	{0x7,0x4,0x7,0x5,0x7},{0x7,0x1,0x1,0x1,0x1},{0x7,0x5,0x7,0x5,0x7},
	{0x7,0x5,0x7,0x1,0x7}
};

// --- static member definitions ---
int FlappyPuzzle::canvasW = 0;
int FlappyPuzzle::canvasH = 0;
float FlappyPuzzle::birdX = 0;
float FlappyPuzzle::birdY = 0;
float FlappyPuzzle::birdVY = 0;
float FlappyPuzzle::birdR = 0;
float FlappyPuzzle::hoverT = 0;
FlappyPuzzle::Pipe FlappyPuzzle::pipes[FlappyPuzzle::MAX_PIPES];
int FlappyPuzzle::pipeCount = 0;
float FlappyPuzzle::pipeTimer = 0;
int FlappyPuzzle::score = 0;
int FlappyPuzzle::phase = PHASE_WAIT;
float FlappyPuzzle::gravity = 0;
float FlappyPuzzle::flapVel = 0;
float FlappyPuzzle::pipeSpeed = 0;
float FlappyPuzzle::pipeInterval = 0;
float FlappyPuzzle::pipeGap = 0;
float FlappyPuzzle::pipeW = 0;
float FlappyPuzzle::groundH = 0;

FlappyPuzzle::FlappyPuzzle() {}
FlappyPuzzle::~FlappyPuzzle() {}

const char* FlappyPuzzle::GetId() { return "flappy"; }
const char* FlappyPuzzle::GetType() const { return "flappy"; }
const char* FlappyPuzzle::GetName() { return "像素小鸟"; }

void FlappyPuzzle::ComputeLayout() {
	birdR = max(8.0f, canvasH / 20.0f);
	groundH = max(10.0f, canvasH / 18.0f);
	pipeW = max(30.0f, canvasW / 8.0f);
	pipeGap = max(80.0f, canvasH / 2.8f);
	gravity = canvasH * 1.8f;
	flapVel = -canvasH * 0.62f;
	pipeSpeed = canvasW * 0.22f;
	pipeInterval = 1.8f;
	birdX = canvasW * 0.25f;
}

void FlappyPuzzle::SpawnPipe() {
	if (pipeCount >= MAX_PIPES) return;
	float playH = canvasH - groundH;
	float margin = birdR * 2.0f;
	float minY = margin + pipeGap / 2.0f;
	float maxY = playH - margin - pipeGap / 2.0f;
	float gapY = (maxY > minY)
				 ? minY + static_cast<float>(rand() % static_cast<int>(maxY - minY))
				 : playH / 2.0f;
	Pipe& p = pipes[pipeCount++];
	p.x = static_cast<float>(canvasW);
	p.gapY = gapY;
	p.gapH = pipeGap;
	p.w = pipeW;
	p.passed = false;
}

void FlappyPuzzle::Init(int width, int height, PostHandle* post) {
	canvasW = width;
	canvasH = height;
	ComputeLayout();
	birdY = canvasH * 0.45f;
	birdVY = 0.0f;
	hoverT = 0.0f;
	pipeCount = 0;
	pipeTimer = pipeInterval * 0.5f;
	score = 0;
	phase = PHASE_WAIT;
	memset(pipes, 0, sizeof(pipes));
}

bool FlappyPuzzle::CircleRect(float cx, float cy, float cr,
			       float rx1, float ry1, float rx2, float ry2) {
	float nx = max(rx1, min(cx, rx2));
	float ny = max(ry1, min(cy, ry2));
	float dx = cx - nx, dy = cy - ny;
	return dx * dx + dy * dy < cr * cr;
}

bool FlappyPuzzle::Collides() {
	float playH = canvasH - groundH;
	if (birdY - birdR < 0.0f || birdY + birdR > playH) return true;
	for (int i = 0; i < pipeCount; i++) {
		const Pipe& p = pipes[i];
		float topBot = p.gapY - p.gapH / 2.0f;
		float botTop = p.gapY + p.gapH / 2.0f;
		if (CircleRect(birdX, birdY, birdR, p.x, 0, p.x + p.w, topBot)) return true;
		if (CircleRect(birdX, birdY, birdR, p.x, botTop, p.x + p.w, playH )) return true;
	}
	return false;
}

void FlappyPuzzle::DrawNumber(Canvas* canvas, int num, int x, int y, int scale) {
	if (num < 0) num = 0;
	char digits[7]; int len = 0;
	if (num == 0) { digits[len++] = 0; }
	else {
		int tmp = num;
		while (tmp > 0 && len < 6) { digits[len++] = static_cast<char>(tmp % 10); tmp /= 10; }
		for (int i = 0, j = len - 1; i < j; i++, --j)
			{ char t = digits[i]; digits[i] = digits[j]; digits[j] = t; }
	}
	for (int d = 0; d < len; d++) {
		int dx = x + d * (3 * scale + scale);
		for (int row = 0; row < 5; row++)
			for (int col = 0; col < 3; col++)
				if (DIGIT_FONT[static_cast<int>(digits[d])][row] & (1 << (2 - col)))
					canvas->PutRect(dx + col * scale, y + row * scale,
									dx + col * scale + scale - 1, y + row * scale + scale - 1, true);
	}
}

void FlappyPuzzle::DrawScene(Canvas* canvas) {
	float playH = canvasH - groundH;

	// Sky
	canvas->SetColor(80, 180, 240);
	canvas->SetAlpha(255);
	canvas->PutRect(0, 0, canvasW - 1, static_cast<int>(playH) - 1, true);

	// Ground
	canvas->SetColor(100, 70, 30);
	canvas->PutRect(0, static_cast<int>(playH), canvasW - 1, canvasH - 1, true);
	canvas->SetColor(60, 160, 60);
	canvas->PutRect(0, static_cast<int>(playH), canvasW - 1, static_cast<int>(playH) + max(3, static_cast<int>(groundH) / 5), true);

	// Pipes
	for (int i = 0; i < pipeCount; i++) {
		const Pipe& p = pipes[i];
		int px1 = static_cast<int>(p.x);
		int px2 = static_cast<int>(p.x + p.w) - 1;
		int topBot = static_cast<int>(p.gapY - p.gapH / 2.0f);
		int botTop = static_cast<int>(p.gapY + p.gapH / 2.0f);
		if (topBot > 0) {
			canvas->SetColor(50, 170, 50);
			canvas->PutRect(px1, 0, px2, topBot - 1, true);
			canvas->SetColor(30, 110, 30);
			canvas->PutRect(px1, 0, px2, topBot - 1, false);
		}
		if (botTop < static_cast<int>(playH)) {
			canvas->SetColor(50, 170, 50);
			canvas->PutRect(px1, botTop, px2, static_cast<int>(playH) - 1, true);
			canvas->SetColor(30, 110, 30);
			canvas->PutRect(px1, botTop, px2, static_cast<int>(playH) - 1, false);
		}
	}

	// Bird
	float drawY = (phase == PHASE_WAIT)
				? birdY + sinf(hoverT * 3.0f) * birdR * 0.5f
				: birdY;
	int bx = static_cast<int>(birdX);
	int by = static_cast<int>(drawY);
	int br = static_cast<int>(birdR);

	canvas->SetColor(255, 210, 40);
	canvas->SetAlpha(255);
	canvas->PutCircle(bx, by, br, true);
	// Eye
	canvas->SetColor(255, 255, 255);
	canvas->PutCircle(bx + br / 3, by - br / 4, max(2, br / 4), true);
	canvas->SetColor(10, 10, 10);
	canvas->PutCircle(bx + br / 3 + 1, by - br / 4, max(1, br / 6), true);
	// Beak
	canvas->SetColor(255, 140, 0);
	int beakS = max(2, br / 4);
	canvas->PutTriangle(bx + br, by - beakS, bx + br, by + beakS,
						bx + br + beakS * 2, by, true);

	// Score
	int dScale = max(2, canvasH / 60);
	canvas->SetColor(255, 255, 255);
	canvas->SetAlpha(255);
	DrawNumber(canvas, score, dScale * 2, dScale * 2, dScale);

	// Wait hint: space bar indicator
	if (phase == PHASE_WAIT) {
		int hW = canvasW / 4;
		int hH = max(5, canvasH / 35);
		int hX = canvasW / 2 - hW / 2;
		int hY = static_cast<int>(drawY + birdR * 2.5f);
		canvas->SetColor(255, 255, 255);
		canvas->SetAlpha(150);
		canvas->PutRect(hX, hY, hX + hW - 1, hY + hH - 1, true);
	}

	// Dead overlay
	if (phase == PHASE_DEAD) {
		canvas->SetColor(0, 0, 0);
		canvas->SetAlpha(140);
		canvas->PutRect(0, 0, canvasW - 1, canvasH - 1, true);

		int sc = max(4, canvasH / 20);
		int numW = 4 * (3 * sc + sc);
		canvas->SetColor(255, 220, 60);
		canvas->SetAlpha(255);
		DrawNumber(canvas, score, canvasW / 2 - numW / 2, canvasH / 2 - sc * 3, sc);
	}
}

int FlappyPuzzle::Loop(Canvas* canvas, int ms, PostHandle* post) {
	if (!canvas) return 0;

	if (canvas->GetWidth() != canvasW || canvas->GetHeight() != canvasH) {
		canvasW = canvas->GetWidth();
		canvasH = canvas->GetHeight();
		ComputeLayout();
	}

	float dt = ms / 1000.0f;

	// Dead: wait for any key
	if (phase == PHASE_DEAD) {
		int key;
		while ((key = canvas->BiosKey(0)) != 0) {
			if (!(key & 0x8000)) {
				DrawScene(canvas);
				return score >= 2 ? 1 : -1;
			}
		}
		DrawScene(canvas);
		return 0;
	}

	// Input (wait and play)
	int key;
	while ((key = canvas->BiosKey(0)) != 0) {
		if (!(key & 0x8000)) {
			int code = key & ~0x8000;
			if (code == canvas->KeyCode("space") || code == canvas->KeyCode("up")) {
				if (phase == PHASE_WAIT) phase = PHASE_PLAY;
				birdVY = flapVel;
			}
		}
	}

	if (phase == PHASE_WAIT) {
		hoverT += dt;
		DrawScene(canvas);
		return 0;
	}

	// Physics
	birdVY += gravity * dt;
	birdY += birdVY * dt;

	// Pipe spawning
	pipeTimer -= dt;
	if (pipeTimer <= 0.0f) {
		SpawnPipe();
		pipeTimer = pipeInterval;
	}

	// Move pipes and score
	for (int i = 0; i < pipeCount; i++) {
		pipes[i].x -= pipeSpeed * dt;
		if (!pipes[i].passed && pipes[i].x + pipes[i].w < birdX) {
			pipes[i].passed = true;
			++score;
		}
	}

	// Remove pipes that have scrolled off screen
	int alive = 0;
	for (int i = 0; i < pipeCount; i++)
		if (pipes[i].x + pipes[i].w > 0.0f)
			pipes[alive++] = pipes[i];
	pipeCount = alive;

	// Collision
	if (Collides())
		phase = PHASE_DEAD;

	DrawScene(canvas);
	return 0;
}
