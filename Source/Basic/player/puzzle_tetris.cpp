#include "puzzle_tetris.h"
#include "player/canvas.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>


using namespace std;

// [type-1][rotation][cell][x,y] within a 4x4 bounding box
static const int PIECES[7][4][4][2] = {
	// I (type 1, cyan)
	{{{0,1},{1,1},{2,1},{3,1}}, {{2,0},{2,1},{2,2},{2,3}}, {{0,2},{1,2},{2,2},{3,2}}, {{1,0},{1,1},{1,2},{1,3}}},
	// O (type 2, yellow)
	{{{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}},
	// T (type 3, purple)
	{{{1,0},{0,1},{1,1},{2,1}}, {{0,0},{0,1},{1,1},{0,2}}, {{0,0},{1,0},{2,0},{1,1}}, {{1,0},{0,1},{1,1},{1,2}}},
	// S (type 4, green)
	{{{1,0},{2,0},{0,1},{1,1}}, {{0,0},{0,1},{1,1},{1,2}}, {{1,0},{2,0},{0,1},{1,1}}, {{0,0},{0,1},{1,1},{1,2}}},
	// Z (type 5, red)
	{{{0,0},{1,0},{1,1},{2,1}}, {{1,0},{0,1},{1,1},{0,2}}, {{0,0},{1,0},{1,1},{2,1}}, {{1,0},{0,1},{1,1},{0,2}}},
	// J (type 6, blue)
	{{{0,0},{0,1},{1,1},{2,1}}, {{0,0},{1,0},{0,1},{0,2}}, {{0,0},{1,0},{2,0},{2,1}}, {{1,0},{1,1},{0,2},{1,2}}},
	// L (type 7, orange)
	{{{2,0},{0,1},{1,1},{2,1}}, {{0,0},{0,1},{0,2},{1,2}}, {{0,0},{1,0},{2,0},{0,1}}, {{0,0},{1,0},{1,1},{1,2}}},
};

static const uint8_t PIECE_COLORS[8][3] = {
	{ 0, 0, 0}, // 0: empty
	{ 0, 255, 255}, // 1: I cyan
	{255, 255, 0}, // 2: O yellow
	{160, 0, 255}, // 3: T purple
	{ 0, 200, 0}, // 4: S green
	{255, 0, 0}, // 5: Z red
	{ 0, 80, 255}, // 6: J blue
	{255, 160, 0}, // 7: L orange
};

static const int SCORE_TABLE[5] = {0, 100, 300, 500, 800};

static const uint8_t DIGIT_FONT[10][5] = {
	{0x7, 0x5, 0x5, 0x5, 0x7}, // 0
	{0x2, 0x6, 0x2, 0x2, 0x7}, // 1
	{0x7, 0x1, 0x7, 0x4, 0x7}, // 2
	{0x7, 0x1, 0x3, 0x1, 0x7}, // 3
	{0x5, 0x5, 0x7, 0x1, 0x1}, // 4
	{0x7, 0x4, 0x7, 0x1, 0x7}, // 5
	{0x7, 0x4, 0x7, 0x5, 0x7}, // 6
	{0x7, 0x1, 0x1, 0x1, 0x1}, // 7
	{0x7, 0x5, 0x7, 0x5, 0x7}, // 8
	{0x7, 0x5, 0x7, 0x1, 0x7}, // 9
};

// Static state
int TetrisPuzzle::cellSize = 0;
int TetrisPuzzle::boardX = 0;
int TetrisPuzzle::boardY = 0;
int TetrisPuzzle::canvasW = 0;
int TetrisPuzzle::canvasH = 0;
int TetrisPuzzle::digitScale = 1;
int TetrisPuzzle::board[TetrisPuzzle::ROWS][TetrisPuzzle::COLS];
int TetrisPuzzle::currType = 1;
int TetrisPuzzle::currRot = 0;
int TetrisPuzzle::currX = 3;
int TetrisPuzzle::currY = -1;
int TetrisPuzzle::nextType = 1;
int TetrisPuzzle::score = 0;
int TetrisPuzzle::level = 1;
int TetrisPuzzle::lines = 0;
float TetrisPuzzle::gravTimer = 0.0f;
int TetrisPuzzle::dasDir = 0;
int TetrisPuzzle::dasTimer = 0;
bool TetrisPuzzle::gameOver = false;
bool TetrisPuzzle::paused = false;
bool TetrisPuzzle::cleared = false;

static constexpr int GOAL_SCORE = 1000;

TetrisPuzzle::TetrisPuzzle() {}
TetrisPuzzle::~TetrisPuzzle() {}

const char* TetrisPuzzle::GetId() { return "tetris"; }
const char* TetrisPuzzle::GetType() const { return "tetris"; }
const char* TetrisPuzzle::GetName() { return "俄罗斯方块"; }

int TetrisPuzzle::RandType() { return rand() % 7 + 1; }
int TetrisPuzzle::GravInterval(){ return max(80, 800 - (level - 1) * 70); }

void TetrisPuzzle::ComputeLayout(int width, int height) {
	canvasW = width;
	canvasH = height;
	int fromH = height / ROWS;
	int fromW = width / (COLS + 6);
	cellSize = max(4, min(fromH, fromW));
	boardX = (width - COLS * cellSize) / 2;
	boardY = (height - ROWS * cellSize) / 2;
	digitScale = max(1, cellSize / 8);
}

void TetrisPuzzle::Init(int width, int height) {
	ComputeLayout(width, height);

	memset(board, 0, sizeof(board));
	score = 0;
	level = 1;
	lines = 0;
	gravTimer = 0.0f;
	dasDir = 0;
	dasTimer = 0;
	gameOver = false;
	paused = false;
	cleared = false;
	nextType = RandType();
	SpawnPiece();
}

void TetrisPuzzle::SpawnPiece() {
	currType = nextType;
	currRot = 0;
	currX = 3;
	currY = -1;
	nextType = RandType();
	if (!CanPlace(currType, currRot, currX, currY))
		gameOver = true;
}

bool TetrisPuzzle::CanPlace(int type, int rot, int px, int py) {
	const int (*cells)[2] = PIECES[type - 1][rot];
	for (int i = 0; i < 4; i++) {
		int cx = px + cells[i][0];
		int cy = py + cells[i][1];
		if (cx < 0 || cx >= COLS || cy >= ROWS) return false;
		if (cy >= 0 && board[cy][cx]) return false;
	}
	return true;
}

void TetrisPuzzle::LockPiece() {
	const int (*cells)[2] = PIECES[currType - 1][currRot];
	for (int i = 0; i < 4; i++) {
		int cx = currX + cells[i][0];
		int cy = currY + cells[i][1];
		if (cy >= 0 && cy < ROWS && cx >= 0 && cx < COLS)
			board[cy][cx] = currType;
	}
}

int TetrisPuzzle::ClearLines() {
	int cleared = 0;
	for (int row = ROWS - 1; row >= 0; ) {
		bool full = true;
		for (int col = 0; col < COLS; col++)
			if (!board[row][col]) { full = false; break; }
		if (full) {
			for (int r = row; r > 0; --r)
				memcpy(board[r], board[r - 1], sizeof(board[0]));
			memset(board[0], 0, sizeof(board[0]));
			++cleared;
		} else {
			--row;
		}
	}
	return cleared;
}

int TetrisPuzzle::GhostY() {
	int gy = currY;
	while (CanPlace(currType, currRot, currX, gy + 1)) ++gy;
	return gy;
}

void TetrisPuzzle::DrawCell(Canvas* canvas, int gx, int gy, int type, uint8_t alpha) {
	if (type <= 0 || type > 7) return;
	const uint8_t* c = PIECE_COLORS[type];
	int x1 = boardX + gx * cellSize;
	int y1 = boardY + gy * cellSize;
	int x2 = x1 + cellSize - 1;
	int y2 = y1 + cellSize - 1;
	canvas->SetColor(c[0], c[1], c[2]);
	canvas->SetAlpha(alpha);
	canvas->PutRect(x1, y1, x2, y2, true);
	uint8_t hr = static_cast<uint8_t>(min(255, static_cast<int>(c[0]) + 80));
	uint8_t hg = static_cast<uint8_t>(min(255, static_cast<int>(c[1]) + 80));
	uint8_t hb = static_cast<uint8_t>(min(255, static_cast<int>(c[2]) + 80));
	canvas->SetColor(hr, hg, hb);
	canvas->PutLine(x1, y1, x2 - 1, y1);
	canvas->PutLine(x1, y1, x1, y2 - 1);
	canvas->SetColor(0, 0, 0);
	canvas->SetAlpha(alpha);
	canvas->PutRect(x1, y1, x2, y2, false);
}

void TetrisPuzzle::DrawPiece(Canvas* canvas, int type, int rot, int px, int py, uint8_t alpha) {
	const int (*cells)[2] = PIECES[type - 1][rot];
	for (int i = 0; i < 4; i++) {
		int gx = px + cells[i][0];
		int gy = py + cells[i][1];
		if (gy >= 0) DrawCell(canvas, gx, gy, type, alpha);
	}
}

void TetrisPuzzle::DrawNumber(Canvas* canvas, int num, int x, int y, int scale) {
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
		for (int r = 0; r < 5; r++)
			for (int c = 0; c < 3; c++)
				if (DIGIT_FONT[static_cast<int>(digits[d])][r] & (1 << (2 - c)))
					canvas->PutRect(dx + c * scale, y + r * scale,
									dx + c * scale + scale - 1, y + r * scale + scale - 1, true);
	}
}

void TetrisPuzzle::DrawBoard(Canvas* canvas) {
	const int boardW = COLS * cellSize;
	const int boardH = ROWS * cellSize;

	// Background
	canvas->SetColor(0, 0, 0);
	canvas->SetAlpha(255);
	canvas->PutRect(0, 0, canvasW - 1, canvasH - 1, true);

	// Board background
	canvas->SetColor(20, 20, 20);
	canvas->PutRect(boardX, boardY, boardX + boardW - 1, boardY + boardH - 1, true);

	// Grid lines
	canvas->SetColor(40, 40, 40);
	canvas->SetAlpha(255);
	for (int c = 1; c < COLS; c++)
		canvas->PutLine(boardX + c * cellSize, boardY, boardX + c * cellSize, boardY + boardH - 1);
	for (int r = 1; r < ROWS; r++)
		canvas->PutLine(boardX, boardY + r * cellSize, boardX + boardW - 1, boardY + r * cellSize);

	// Locked cells
	for (int row = 0; row < ROWS; row++)
		for (int col = 0; col < COLS; col++)
			if (board[row][col])
				DrawCell(canvas, col, row, board[row][col], 255);

	// Ghost piece
	int gy = GhostY();
	if (!gameOver && gy != currY)
		DrawPiece(canvas, currType, currRot, currX, gy, 55);

	// Current piece
	if (!gameOver)
		DrawPiece(canvas, currType, currRot, currX, currY, 255);

	// Board border
	canvas->SetColor(160, 160, 160);
	canvas->SetAlpha(255);
	canvas->PutRect(boardX - 1, boardY - 1, boardX + boardW, boardY + boardH, false);
	canvas->PutRect(boardX - 2, boardY - 2, boardX + boardW + 1, boardY + boardH + 1, false);

	// --- Right panel: next piece preview ---
	int margin = max(2, cellSize / 4);
	int prevX = boardX + boardW + margin;
	int prevY = boardY + cellSize;
	int prevSz = 4 * cellSize;

	if (prevX + prevSz < canvasW) {
		canvas->SetColor(60, 60, 60);
		canvas->PutRect(prevX, prevY, prevX + prevSz - 1, prevY + prevSz - 1, true);
		canvas->SetColor(160, 160, 160);
		canvas->PutRect(prevX - 1, prevY - 1, prevX + prevSz, prevY + prevSz, false);

		const int (*ncells)[2] = PIECES[nextType - 1][0];
		const uint8_t* nc = PIECE_COLORS[nextType];
		for (int i = 0; i < 4; i++) {
			int nx = prevX + ncells[i][0] * cellSize;
			int ny = prevY + ncells[i][1] * cellSize;
			canvas->SetColor(nc[0], nc[1], nc[2]);
			canvas->SetAlpha(255);
			canvas->PutRect(nx, ny, nx + cellSize - 1, ny + cellSize - 1, true);
			canvas->SetColor(0, 0, 0);
			canvas->PutRect(nx, ny, nx + cellSize - 1, ny + cellSize - 1, false);
		}

		// Score / lines / level
		int infoY = prevY + prevSz + cellSize;
		if (infoY + 5 * digitScale < canvasH) {
			canvas->SetColor(200, 200, 200); canvas->SetAlpha(255);
			DrawNumber(canvas, score, prevX, infoY, digitScale);
			infoY += 5 * digitScale + digitScale * 3;

			canvas->SetColor(0, 220, 220);
			DrawNumber(canvas, lines, prevX, infoY, digitScale);
			infoY += 5 * digitScale + digitScale * 3;

			canvas->SetColor(255, 220, 0);
			DrawNumber(canvas, level, prevX, infoY, digitScale);
		}
	}

	// --- Left panel: level bar ---
	int lPanelW = boardX - 2 * margin;
	int lPanelX = margin;
	if (lPanelW >= 4) {
		canvas->SetColor(80, 80, 80);
		canvas->SetAlpha(255);
		canvas->PutRect(lPanelX, boardY, lPanelX + lPanelW - 1, boardY + boardH - 1, true);
		canvas->SetColor(100, 100, 100);
		canvas->PutRect(lPanelX, boardY, lPanelX + lPanelW - 1, boardY + boardH - 1, false);

		int barMaxH = boardH - 2;
		int barH = min(barMaxH, (level - 1) * (boardH / 20));
		const uint8_t* lc = PIECE_COLORS[((level - 1) % 7) + 1];
		canvas->SetColor(lc[0], lc[1], lc[2]);
		canvas->SetAlpha(120);
		if (barH > 0)
			canvas->PutRect(lPanelX + 1, boardY + boardH - 1 - barH,
							lPanelX + lPanelW - 2, boardY + boardH - 2, true);
		canvas->SetAlpha(255);
	}

	// 目标分数提示（右侧面板底部）
	if (prevX + prevSz < canvasW) {
		int goalY = canvasH - 5 * digitScale - margin;
		if (goalY > 0) {
			canvas->SetColor(100, 100, 100);
			canvas->SetAlpha(255);
			DrawNumber(canvas, GOAL_SCORE, prevX, goalY, digitScale);
			// 进度条
			int barW = 4 * cellSize;
			int barH3 = max(2, digitScale);
			int filled = min(barW, barW * score / GOAL_SCORE);
			canvas->SetColor(40, 40, 40);
			canvas->PutRect(prevX, goalY - barH3 * 2 - 1,
							prevX + barW - 1, goalY - barH3 - 2, true);
			if (filled > 0) {
				canvas->SetColor(0, 200, 100);
				canvas->PutRect(prevX, goalY - barH3 * 2 - 1,
								prevX + filled - 1, goalY - barH3 - 2, true);
			}
		}
	}

	// Game over overlay
	if (gameOver) {
		int midY = boardY + boardH / 2;
		canvas->SetColor(180, 0, 0);
		canvas->SetAlpha(200);
		canvas->PutRect(boardX + cellSize, midY - cellSize,
						boardX + boardW - cellSize - 1, midY + cellSize - 1, true);
		canvas->SetColor(255, 255, 255);
		canvas->SetAlpha(255);
		DrawNumber(canvas, score, boardX + cellSize * 2, midY - digitScale * 2, digitScale);
	}

	// Cleared overlay
	if (cleared) {
		canvas->SetColor(0, 0, 0);
		canvas->SetAlpha(160);
		canvas->PutRect(boardX, boardY, boardX + boardW - 1, boardY + boardH - 1, true);
		// 金色边框
		canvas->SetColor(255, 200, 0);
		canvas->SetAlpha(255);
		int bx1 = boardX + cellSize, by1 = boardY + boardH / 2 - cellSize * 2;
		int bx2 = boardX + boardW - cellSize - 1, by2 = boardY + boardH / 2 + cellSize * 2 - 1;
		canvas->PutRect(bx1, by1, bx2, by2, true);
		canvas->SetColor(0, 0, 0);
		canvas->PutRect(bx1 + 2, by1 + 2, bx2 - 2, by2 - 2, true);
		// 最终得分
		canvas->SetColor(255, 220, 0);
		int sc = digitScale + 1;
		int numW = 6 * (3 * sc + sc);
		DrawNumber(canvas, score, (boardX + boardW / 2) - numW / 2, boardY + boardH / 2 - sc * 3, sc);
	}

	// Pause overlay
	if (paused) {
		canvas->SetColor(0, 0, 0);
		canvas->SetAlpha(150);
		canvas->PutRect(boardX, boardY, boardX + boardW - 1, boardY + boardH - 1, true);
		int barW = max(2, cellSize / 2);
		int barH2 = cellSize * 3;
		int cx = boardX + boardW / 2;
		int cy = boardY + boardH / 2;
		canvas->SetColor(255, 255, 0);
		canvas->SetAlpha(255);
		canvas->PutRect(cx - barW * 2, cy - barH2 / 2, cx - barW - 1, cy + barH2 / 2, true);
		canvas->PutRect(cx + barW, cy - barH2 / 2, cx + barW * 2 - 1, cy + barH2 / 2, true);
	}
}

int TetrisPuzzle::Loop(Canvas* canvas, int ms) {
	if (!canvas) return 0;

	// 自动适配画布尺寸，无需依赖 Init 的调用时机
	if (canvas->GetWidth() != canvasW || canvas->GetHeight() != canvasH)
		ComputeLayout(canvas->GetWidth(), canvas->GetHeight());

	// 通关等待任意键
	if (cleared) {
		int key;
		while ((key = canvas->BiosKey(0)) != 0) {
			if (!(key & 0x8000)) {
				DrawBoard(canvas);
				return 1;
			}
		}
		DrawBoard(canvas);
		return 0;
	}

	int key;
	while ((key = canvas->BiosKey(0)) != 0) {
		bool down = !(key & 0x8000);
		int code = key & ~0x8000;

		if (gameOver) {
			if (down) {
				DrawBoard(canvas);
				return -1;
			}
			continue;
		}

		if (down && code == canvas->KeyCode("escape")) {
			paused = !paused;
			continue;
		}

		if (paused) continue;

		if (down) {
			if (code == canvas->KeyCode("left")) {
				if (CanPlace(currType, currRot, currX - 1, currY)) --currX;
				dasDir = -1; dasTimer = 180;
			}
			else if (code == canvas->KeyCode("right")) {
				if (CanPlace(currType, currRot, currX + 1, currY)) ++currX;
				dasDir = 1; dasTimer = 180;
			}
			else if (code == canvas->KeyCode("up")) {
				int nr = (currRot + 1) % 4;
				if (CanPlace(currType, nr, currX, currY)) currRot = nr;
				else if (CanPlace(currType, nr, currX - 1, currY)) { currRot = nr; --currX; }
				else if (CanPlace(currType, nr, currX + 1, currY)) { currRot = nr; ++currX; }
				else if (CanPlace(currType, nr, currX - 2, currY)) { currRot = nr; currX -= 2; }
				else if (CanPlace(currType, nr, currX + 2, currY)) { currRot = nr; currX += 2; }
			}
			else if (code == canvas->KeyCode("down")) {
				if (CanPlace(currType, currRot, currX, currY + 1)) {
					++currY; score += 1;
				} else {
					LockPiece();
					int cleared = ClearLines();
					score += SCORE_TABLE[cleared] * level;
					lines += cleared;
					level = lines / 10 + 1;
					gravTimer = 0.0f;
					SpawnPiece();
				}
			}
			else if (code == canvas->KeyCode("space")) {
				int gy2 = GhostY();
				score += (gy2 - currY) * 2;
				currY = gy2;
				LockPiece();
				int cleared = ClearLines();
				score += SCORE_TABLE[cleared] * level;
				lines += cleared;
				level = lines / 10 + 1;
				gravTimer = 0.0f;
				SpawnPiece();
			}
		}
		else {
			if ((code == canvas->KeyCode("left") && dasDir == -1) ||
				(code == canvas->KeyCode("right") && dasDir == 1)) {
				dasDir = 0; dasTimer = 0;
			}
		}
	}

	if (gameOver || paused) {
		DrawBoard(canvas);
		return 0;
	}

	// DAS
	if (dasDir != 0) {
		dasTimer -= ms;
		while (dasTimer <= 0) {
			dasTimer += 50;
			if (dasDir == -1) { if (CanPlace(currType, currRot, currX - 1, currY)) --currX; }
			else { if (CanPlace(currType, currRot, currX + 1, currY)) ++currX; }
		}
	}

	// Gravity
	gravTimer += static_cast<float>(ms);
	int gi = GravInterval();
	while (gravTimer >= static_cast<float>(gi)) {
		gravTimer -= static_cast<float>(gi);
		if (CanPlace(currType, currRot, currX, currY + 1)) {
			++currY;
		} else {
			LockPiece();
			int cleared = ClearLines();
			score += SCORE_TABLE[cleared] * level;
			lines += cleared;
			level = lines / 10 + 1;
			gravTimer = 0.0f;
			SpawnPiece();
			if (gameOver) break;
		}
	}

	// 通关检测
	if (!cleared && score >= GOAL_SCORE)
		cleared = true;

	DrawBoard(canvas);
	return 0;
}
