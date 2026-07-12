#include "puzzle_dance.h"
#include "player/canvas.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>


using namespace std;

static constexpr int PHASE_INPUT = 0;
static constexpr int PHASE_RESULT = 1;
static constexpr int PHASE_DONE = 2;
static constexpr float PERF_FRAC0 = 0.75f; // perfect zone start
static constexpr float PERF_FRAC1 = 0.88f; // perfect zone end
static constexpr float ROUND_DUR_MS = 5000.0f;
static constexpr float RESULT_DUR_MS = 1200.0f;
static constexpr float FLASH_DUR_MS = 280.0f;

static const uint8_t DIGIT_FONT[10][5] = {
	{0x7,0x5,0x5,0x5,0x7},{0x2,0x6,0x2,0x2,0x7},{0x7,0x1,0x7,0x4,0x7},
	{0x7,0x1,0x3,0x1,0x7},{0x5,0x5,0x7,0x1,0x1},{0x7,0x4,0x7,0x1,0x7},
	{0x7,0x4,0x7,0x5,0x7},{0x7,0x1,0x1,0x1,0x1},{0x7,0x5,0x7,0x5,0x7},
	{0x7,0x5,0x7,0x1,0x7}
};

// --- static member definitions ---
int DancePuzzle::canvasW = 0;
int DancePuzzle::canvasH = 0;
int DancePuzzle::round = 1;
int DancePuzzle::arrowCount = 3;
int DancePuzzle::arrows[DancePuzzle::MAX_ARROWS];
int DancePuzzle::inputPos = 0;
bool DancePuzzle::inputDone = false;
int DancePuzzle::results[DancePuzzle::TOTAL_ROUNDS];
int DancePuzzle::totalScore = 0;
float DancePuzzle::elapsed = 0.0f;
float DancePuzzle::roundDuration = ROUND_DUR_MS;
int DancePuzzle::phase = PHASE_INPUT;
float DancePuzzle::phaseTimer = 0.0f;
bool DancePuzzle::wrongFlash = false;
float DancePuzzle::wrongFlashTimer = 0.0f;

DancePuzzle::DancePuzzle() {}
DancePuzzle::~DancePuzzle() {}

const char* DancePuzzle::GetId() { return "dance"; }
const char* DancePuzzle::GetType() const { return "dance"; }
const char* DancePuzzle::GetName() { return "节奏舞步"; }

int DancePuzzle::ArrowCountForRound(int rnd) {
	return 3 + (rnd - 1) * 5 / 19; // 3 at round 1, 8 at round 20
}

void DancePuzzle::StartRound() {
	arrowCount = ArrowCountForRound(round);
	for (int i = 0; i < arrowCount; i++)
		arrows[i] = rand() % 4;
	inputPos = 0;
	inputDone = false;
	elapsed = 0.0f;
	roundDuration = ROUND_DUR_MS;
	phase = PHASE_INPUT;
	wrongFlash = false;
	wrongFlashTimer = 0.0f;
}

void DancePuzzle::Init(int width, int height) {
	canvasW = width;
	canvasH = height;
	round = 1;
	totalScore = 0;
	memset(results, 0, sizeof(results));
	StartRound();
}

void DancePuzzle::DrawArrow(Canvas* canvas, int type, int cx, int cy, int sz,
			     uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	canvas->SetColor(r, g, b);
	canvas->SetAlpha(a);
	int h = sz / 2;
	int w = sz / 3;
	int s = max(1, sz / 8);
	switch (type) {
	case 0: // up
		canvas->PutTriangle(cx, cy - h, cx - w, cy, cx + w, cy, true);
		canvas->PutRect(cx - s, cy, cx + s, cy + h, true);
		break;
	case 1: // down
		canvas->PutTriangle(cx, cy + h, cx - w, cy, cx + w, cy, true);
		canvas->PutRect(cx - s, cy - h, cx + s, cy, true);
		break;
	case 2: // left
		canvas->PutTriangle(cx - h, cy, cx, cy - w, cx, cy + w, true);
		canvas->PutRect(cx, cy - s, cx + h, cy + s, true);
		break;
	case 3: // right
		canvas->PutTriangle(cx + h, cy, cx, cy - w, cx, cy + w, true);
		canvas->PutRect(cx - h, cy - s, cx, cy + s, true);
		break;
	}
}

void DancePuzzle::DrawNumber(Canvas* canvas, int num, int x, int y, int scale) {
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

void DancePuzzle::DrawScene(Canvas* canvas) {
	// Background
	canvas->SetColor(10, 10, 20);
	canvas->SetAlpha(255);
	canvas->PutRect(0, 0, canvasW - 1, canvasH - 1, true);

	// ── Time bar (bottom) ──
	int barH = max(14, canvasH / 18);
	int barY = canvasH - barH;

	canvas->SetColor(30, 30, 30);
	canvas->PutRect(0, barY, canvasW - 1, canvasH - 1, true);

	float frac = min(1.0f, elapsed / roundDuration);
	int filled = static_cast<int>(frac * canvasW);
	uint8_t barR = static_cast<uint8_t>(frac < 0.5f ? frac * 2.0f * 255 : 255);
	uint8_t barG = static_cast<uint8_t>(frac < 0.5f ? 200 : (1.0f - frac) * 2.0f * 200);
	canvas->SetColor(barR, barG, 0);
	canvas->SetAlpha(255);
	if (filled > 0)
		canvas->PutRect(0, barY + 2, filled - 1, canvasH - 3, true);

	// Perfect zone
	int pzX0 = static_cast<int>(PERF_FRAC0 * canvasW);
	int pzX1 = static_cast<int>(PERF_FRAC1 * canvasW);
	canvas->SetColor(255, 220, 0);
	canvas->SetAlpha(150);
	canvas->PutRect(pzX0, barY, pzX1, canvasH - 1, true);
	canvas->SetAlpha(255);
	canvas->PutLine(pzX0, barY, pzX0, canvasH - 1);
	canvas->PutLine(pzX1, barY, pzX1, canvasH - 1);

	// ── Top bar: round indicator dots ──
	int topH = max(20, canvasH / 12);
	int dotR = max(3, min(topH / 3, canvasW / (TOTAL_ROUNDS * 4)));
	int dotCY = topH / 2;
	int dotSpan = TOTAL_ROUNDS * (dotR * 2 + 3);
	int dotStartX = (canvasW - dotSpan) / 2;

	for (int i = 0; i < TOTAL_ROUNDS; i++) {
		int dx = dotStartX + i * (dotR * 2 + 3) + dotR;
		canvas->SetAlpha(255);
		if (i < round - 1) {
			int res = results[i];
			if (res == 3) canvas->SetColor(255, 200, 0);
			else if (res == 1) canvas->SetColor( 60, 140, 255);
			else canvas->SetColor(120, 0, 0);
		} else if (i == round - 1) {
			canvas->SetColor(255, 255, 255);
		} else {
			canvas->SetColor(50, 50, 55);
		}
		canvas->PutCircle(dx, dotCY, dotR, true);
	}

	// Total score (right of top bar)
	int dScale = max(1, topH / 8);
	canvas->SetColor(180, 180, 180);
	canvas->SetAlpha(255);
	DrawNumber(canvas, totalScore, canvasW - 7 * (3 * dScale + dScale) - 4, dotCY - dScale * 2, dScale);

	// ── Arrow row ──
	int midY = topH + (barY - topH) * 2 / 5;
	int arrowSz = min(canvasW / (MAX_ARROWS + 2), min((barY - topH) * 2 / 5, 80));
	arrowSz = max(16, arrowSz);
	int gap = max(4, arrowSz / 4);

	int totalArW = arrowCount * arrowSz + (arrowCount - 1) * gap;
	int arStartX = (canvasW - totalArW) / 2 + arrowSz / 2;

	for (int i = 0; i < arrowCount; i++) {
		int ax = arStartX + i * (arrowSz + gap);
		uint8_t r, g, b, a;
		if (inputDone || i < inputPos) {
			r = 0; g = 210; b = 90; a = 220;
		} else if (i == inputPos && wrongFlash) {
			r = 255; g = 40; b = 40; a = 255;
		} else if (i == inputPos) {
			r = 255; g = 255; b = 255; a = 255;
		} else {
			r = 80; g = 80; b = 90; a = 150;
		}
		DrawArrow(canvas, arrows[i], ax, midY, arrowSz, r, g, b, a);
	}

	// Space prompt bar (visible after all arrows done)
	if (inputDone && phase == PHASE_INPUT) {
		bool inPerf = frac >= PERF_FRAC0 && frac < PERF_FRAC1;
		int sW = arrowSz * 3;
		int sH = max(6, arrowSz / 5);
		int sX = canvasW / 2 - sW / 2;
		int sY = midY + arrowSz / 2 + gap * 2;
		if (inPerf) canvas->SetColor(255, 220, 0);
		else canvas->SetColor(120, 120, 200);
		canvas->SetAlpha(200);
		canvas->PutRect(sX, sY, sX + sW - 1, sY + sH - 1, true);
	}

	// ── Result banner (PHASE_RESULT) ──
	if (phase == PHASE_RESULT) {
		int res = results[round - 1];
		int bW = canvasW / 2;
		int bH = max(24, (barY - topH) / 6);
		int bX = (canvasW - bW) / 2;
		int bY2 = topH + (barY - topH) * 3 / 4 - bH / 2;
		if (res == 3) canvas->SetColor(200, 160, 0);
		else if (res == 1) canvas->SetColor( 30, 80, 200);
		else canvas->SetColor(150, 20, 20);
		canvas->SetAlpha(230);
		canvas->PutRect(bX, bY2, bX + bW - 1, bY2 + bH - 1, true);
		canvas->SetColor(255, 255, 255);
		canvas->SetAlpha(255);
		canvas->PutRect(bX + 1, bY2 + 1, bX + bW - 2, bY2 + bH - 2, false);
	}

	// ── Final screen (PHASE_DONE) ──
	if (phase == PHASE_DONE) {
		canvas->SetColor(0, 0, 0);
		canvas->SetAlpha(190);
		canvas->PutRect(0, 0, canvasW - 1, canvasH - 1, true);

		int sc = max(3, min(10, canvasH / 30));
		int numW = 3 * (3 * sc + sc);
		canvas->SetColor(255, 220, 0);
		canvas->SetAlpha(255);
		DrawNumber(canvas, totalScore, canvasW / 2 - numW / 2, canvasH / 2 - sc * 4, sc);

		int dr = max(5, canvasW / (TOTAL_ROUNDS * 4));
		int span = TOTAL_ROUNDS * (dr * 2 + 4);
		int startX = (canvasW - span) / 2;
		int dotY2 = canvasH / 2 + sc * 8;
		for (int i = 0; i < TOTAL_ROUNDS; i++) {
			int dx = startX + i * (dr * 2 + 4) + dr;
			int res = results[i];
			if (res == 3) canvas->SetColor(255, 200, 0);
			else if (res == 1) canvas->SetColor( 60, 140, 255);
			else canvas->SetColor(120, 0, 0);
			canvas->SetAlpha(255);
			canvas->PutCircle(dx, dotY2, dr, true);
		}
	}
}

int DancePuzzle::Loop(Canvas* canvas, int ms) {
	if (!canvas) return 0;

	if (canvas->GetWidth() != canvasW || canvas->GetHeight() != canvasH) {
		canvasW = canvas->GetWidth();
		canvasH = canvas->GetHeight();
	}

	// ── PHASE_DONE: wait for any key, then return score ──
	if (phase == PHASE_DONE) {
		int key;
		while ((key = canvas->BiosKey(0)) != 0) {
			if (!(key & 0x8000)) {
				DrawScene(canvas);
				return totalScore >= 30 ? 1 : -1;
			}
		}
		DrawScene(canvas);
		return 0;
	}

	// ── PHASE_RESULT: pause between rounds ──
	if (phase == PHASE_RESULT) {
		canvas->ClearKeyBuffer();
		phaseTimer -= static_cast<float>(ms);
		if (phaseTimer <= 0.0f) {
			if (round >= TOTAL_ROUNDS)
				phase = PHASE_DONE;
			else {
				++round;
				StartRound();
			}
		}
		DrawScene(canvas);
		return 0;
	}

	// ── PHASE_INPUT ──

	// Wrong-flash countdown; reset inputPos when it expires
	if (wrongFlash) {
		wrongFlashTimer -= static_cast<float>(ms);
		if (wrongFlashTimer <= 0.0f) {
			wrongFlash = false;
			inputPos = 0;
		}
	}

	// Key input (suppressed during wrong flash)
	if (!wrongFlash) {
		int key;
		while ((key = canvas->BiosKey(0)) != 0) {
			if (!(key & 0x8000)) {
				int code = key & ~0x8000;

				if (inputDone) {
					if (code == canvas->KeyCode("space")) {
						float f = min(1.0f, elapsed / roundDuration);
						int res = (f >= PERF_FRAC0 && f < PERF_FRAC1) ? 3 : 1;
						results[round - 1] = res;
						totalScore += res;
						phase = PHASE_RESULT;
						phaseTimer = RESULT_DUR_MS;
					}
				} else {
					int pressed = -1;
					if (code == canvas->KeyCode("up")) pressed = 0;
					else if (code == canvas->KeyCode("down")) pressed = 1;
					else if (code == canvas->KeyCode("left")) pressed = 2;
					else if (code == canvas->KeyCode("right")) pressed = 3;

					if (pressed >= 0) {
						if (pressed == arrows[inputPos]) {
							++inputPos;
							if (inputPos >= arrowCount)
								inputDone = true;
						} else {
							wrongFlash = true;
							wrongFlashTimer = FLASH_DUR_MS;
						}
					}
				}
			}
		}
	}

	// Time advance; fail if time runs out
	elapsed += static_cast<float>(ms);
	if (phase == PHASE_INPUT && elapsed >= roundDuration) {
		results[round - 1] = 0;
		phase = PHASE_RESULT;
		phaseTimer = RESULT_DUR_MS;
	}

	DrawScene(canvas);
	return 0;
}
