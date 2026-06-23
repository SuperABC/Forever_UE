#include "puzzle_war.h"
#include "player/canvas.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>


using namespace std;

static constexpr int PHASE_PLACE = 0;
static constexpr int PHASE_BATTLE = 1;
static constexpr int PHASE_END = 2;

static const uint8_t DIGIT_FONT[10][5] = {
	{0x7,0x5,0x5,0x5,0x7},{0x2,0x6,0x2,0x2,0x7},{0x7,0x1,0x7,0x4,0x7},
	{0x7,0x1,0x3,0x1,0x7},{0x5,0x5,0x7,0x1,0x1},{0x7,0x4,0x7,0x1,0x7},
	{0x7,0x4,0x7,0x5,0x7},{0x7,0x1,0x1,0x1,0x1},{0x7,0x5,0x7,0x5,0x7},
	{0x7,0x5,0x7,0x1,0x7}
};

// --- ship shapes: {rows, cols} ---
const int WarPuzzle::SHAPES[WarPuzzle::SHIP_COUNT][2] = {
	{1,3},{1,3},{1,4},{1,4},{1,5},{2,3}
};

// --- static member definitions ---
int WarPuzzle::canvasW = 0;
int WarPuzzle::canvasH = 0;
int WarPuzzle::cellSize = 0;
int WarPuzzle::playerBoardX = 0;
int WarPuzzle::playerBoardY = 0;
int WarPuzzle::aiBoardX = 0;
int WarPuzzle::aiBoardY = 0;
WarPuzzle::BoardState WarPuzzle::playerBoard;
WarPuzzle::BoardState WarPuzzle::aiBoard;
int WarPuzzle::placeIdx = 0;
int WarPuzzle::placeCurR = 0;
int WarPuzzle::placeCurC = 0;
int WarPuzzle::placeRot = 0;
int WarPuzzle::cursorR = 0;
int WarPuzzle::cursorC = 0;
bool WarPuzzle::playerTurn = true;
int WarPuzzle::phase = 0;
bool WarPuzzle::playerWon = false;
float WarPuzzle::aiDelay = 0.0f;
int WarPuzzle::playerShots = 0;

WarPuzzle::WarPuzzle() {}
WarPuzzle::~WarPuzzle() {}

const char* WarPuzzle::GetId() { return "war"; }
const char* WarPuzzle::GetType() const { return "war"; }
const char* WarPuzzle::GetName() { return "战舰"; }

// --- layout ---

void WarPuzzle::ComputeLayout() {
	int margin = max(10, min(canvasW, canvasH) / 25);
	int infoH = max(28, canvasH / 10);
	int spacing = max(15, canvasW / 20);

	int availW = canvasW - 2 * margin - spacing;
	int availH = canvasH - 2 * margin - infoH;

	int csW = availW / (2 * BOARD);
	int csH = availH / BOARD;
	cellSize = max(8, min(csW, csH));

	int boardPx = cellSize * BOARD;
	int totalW = 2 * boardPx + spacing;
	int startX = (canvasW - totalW) / 2;
	int startY = margin + infoH;

	playerBoardX = startX;
	playerBoardY = startY;
	aiBoardX = startX + boardPx + spacing;
	aiBoardY = startY;
}

// --- ship geometry ---

void WarPuzzle::GetCells(int si, int rot, int ar, int ac, int out[][2], int& cnt) {
	int rows = SHAPES[si][0], cols = SHAPES[si][1];
	cnt = 0;
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			if (rot == 0) {
				out[cnt][0] = ar + r;
				out[cnt][1] = ac + c;
			} else {
				// 90° CW: new_r=old_c, new_c=(rows-1)-old_r
				out[cnt][0] = ar + c;
				out[cnt][1] = ac + (rows - 1 - r);
			}
			cnt++;
		}
	}
}

bool WarPuzzle::CanPlace(BoardState& b, int si, int rot, int ar, int ac) {
	int cells[MAX_CELLS][2]; int cnt;
	GetCells(si, rot, ar, ac, cells, cnt);
	for (int i = 0; i < cnt; i++) {
		int r = cells[i][0], c = cells[i][1];
		if (r < 0 || r >= BOARD || c < 0 || c >= BOARD) return false;
		if (b.grid[r][c] != -1) return false;
	}
	return true;
}

void WarPuzzle::PlaceShip(BoardState& b, int si, int rot, int ar, int ac) {
	int cells[MAX_CELLS][2]; int cnt;
	GetCells(si, rot, ar, ac, cells, cnt);
	b.ships[si].cellCount = cnt;
	b.ships[si].sunk = false;
	for (int i = 0; i < cnt; i++) {
		int r = cells[i][0], c = cells[i][1];
		b.grid[r][c] = si;
		b.ships[si].cells[i][0] = r;
		b.ships[si].cells[i][1] = c;
	}
}

void WarPuzzle::AIPlace() {
	for (int si = 0; si < SHIP_COUNT; si++) {
		for (int tries = 0; tries < 10000; tries++) {
			int rot = rand() % 2;
			int nr = (rot == 0) ? SHAPES[si][0] : SHAPES[si][1];
			int nc = (rot == 0) ? SHAPES[si][1] : SHAPES[si][0];
			if (nr > BOARD || nc > BOARD) continue;
			int ar = rand() % (BOARD - nr + 1);
			int ac = rand() % (BOARD - nc + 1);
			if (CanPlace(aiBoard, si, rot, ar, ac)) {
				PlaceShip(aiBoard, si, rot, ar, ac);
				break;
			}
		}
	}
}

// --- game logic ---

bool WarPuzzle::FireAt(BoardState& b, int r, int c) {
	if (b.hit[r][c]) return false;
	b.hit[r][c] = true;
	int si = b.grid[r][c];
	if (si >= 0) {
		CheckSunk(b, si);
		return true;
	}
	return false;
}

void WarPuzzle::CheckSunk(BoardState& b, int si) {
	if (b.ships[si].cellCount == 0) return;
	for (int i = 0; i < b.ships[si].cellCount; i++)
		if (!b.hit[b.ships[si].cells[i][0]][b.ships[si].cells[i][1]]) return;
	b.ships[si].sunk = true;
}

bool WarPuzzle::AllSunk(const BoardState& b) {
	for (int i = 0; i < SHIP_COUNT; i++)
		if (!b.ships[i].sunk) return false;
	return true;
}

void WarPuzzle::AIFire() {
	int cands[BOARD * BOARD][2]; int cnt = 0;
	for (int r = 0; r < BOARD; r++)
		for (int c = 0; c < BOARD; c++)
			if (!playerBoard.hit[r][c]) {
				cands[cnt][0] = r;
				cands[cnt][1] = c;
				cnt++;
			}
	if (cnt == 0) return;

	int idx = rand() % cnt;
	bool hit = FireAt(playerBoard, cands[idx][0], cands[idx][1]);

	if (AllSunk(playerBoard)) {
		playerWon = false;
		phase = PHASE_END;
		return;
	}
	if (hit) {
		aiDelay = 0.6f; // pause before AI fires again
	} else {
		playerTurn = true;
	}
}

// --- init ---

void WarPuzzle::Init(int width, int height) {
	canvasW = width;
	canvasH = height;
	ComputeLayout();

	memset(&playerBoard, 0, sizeof(playerBoard));
	memset(&aiBoard, 0, sizeof(aiBoard));
	for (int r = 0; r < BOARD; r++)
		for (int c = 0; c < BOARD; c++) {
			playerBoard.grid[r][c] = -1;
			aiBoard.grid[r][c] = -1;
		}
	for (int i = 0; i < SHIP_COUNT; i++) {
		playerBoard.ships[i].cellCount = 0;
		playerBoard.ships[i].sunk = false;
		aiBoard.ships[i].cellCount = 0;
		aiBoard.ships[i].sunk = false;
	}

	placeIdx = 0;
	placeCurR = 0; placeCurC = 0; placeRot = 0;
	cursorR = 0; cursorC = 0;
	playerTurn = true;
	phase = PHASE_PLACE;
	playerWon = false;
	aiDelay = 0.0f;
	playerShots = 0;

	AIPlace();
}

// --- drawing ---

void WarPuzzle::DrawNumber(Canvas* canvas, int num, int x, int y, int scale) {
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
		int dx = x + d * (4 * scale);
		for (int row = 0; row < 5; row++)
			for (int col = 0; col < 3; col++)
				if (DIGIT_FONT[static_cast<int>(digits[d])][row] & (1 << (2 - col)))
					canvas->PutRect(dx + col * scale, y + row * scale,
									dx + col * scale + scale - 1, y + row * scale + scale - 1, true);
	}
}

void WarPuzzle::DrawBoard(Canvas* canvas, const BoardState& b,
			   int bx, int by, bool hideShips,
			   bool showCursor, int cr, int cc) {
	int boardPx = BOARD * cellSize;

	// background
	canvas->SetColor(15, 25, 50);
	canvas->SetAlpha(255);
	canvas->PutRect(bx - 1, by - 1, bx + boardPx, by + boardPx, true);

	// cells
	for (int r = 0; r < BOARD; r++) {
		for (int c = 0; c < BOARD; c++) {
			int px = bx + c * cellSize + 1;
			int py = by + r * cellSize + 1;
			int x2 = px + cellSize - 3;
			int y2 = py + cellSize - 3;

			int si = b.grid[r][c];
			bool wasHit = b.hit[r][c];
			bool isSunk = (si >= 0 && b.ships[si].sunk);

			if (wasHit) {
				if (si >= 0) {
					// hit on ship
					if (isSunk) {
						canvas->SetColor(230, 140, 30); // orange = sunk
					} else {
						canvas->SetColor(215, 55, 55); // red = hit
					}
					canvas->PutRect(px, py, x2, y2, true);
					// cross mark
					canvas->SetColor(255, 255, 255);
					canvas->SetAlpha(120);
					canvas->PutLine(px + 1, py + 1, x2 - 1, y2 - 1);
					canvas->PutLine(x2 - 1, py + 1, px + 1, y2 - 1);
					canvas->SetAlpha(255);
				} else {
					// miss - water splash
					canvas->SetColor(25, 70, 140);
					canvas->PutRect(px, py, x2, y2, true);
					int mid = cellSize / 2;
					canvas->SetColor(160, 200, 255);
					canvas->PutCircle(px + mid - 1, py + mid - 1, max(1, cellSize / 7), true);
				}
			} else if (si >= 0 && !hideShips) {
				// own ship (player board)
				canvas->SetColor(45, 95, 195);
				canvas->PutRect(px, py, x2, y2, true);
			} else {
				// unknown / empty
				canvas->SetColor(20, 40, 80);
				canvas->PutRect(px, py, x2, y2, true);
			}
		}
	}

	// grid lines
	canvas->SetColor(35, 60, 110);
	canvas->SetAlpha(255);
	for (int i = 0; i <= BOARD; i++) {
		canvas->PutLine(bx, by + i * cellSize, bx + boardPx, by + i * cellSize);
		canvas->PutLine(bx + i * cellSize, by, bx + i * cellSize, by + boardPx);
	}

	// cursor
	if (showCursor && cr >= 0 && cc >= 0) {
		int px = bx + cc * cellSize + 1;
		int py = by + cr * cellSize + 1;
		canvas->SetColor(255, 255, 80);
		canvas->PutRect(px, py, px + cellSize - 3, py + cellSize - 3, false);
	}
}

void WarPuzzle::DrawGhost(Canvas* canvas, int bx, int by,
			   int si, int rot, int ar, int ac) {
	int cells[MAX_CELLS][2]; int cnt;
	GetCells(si, rot, ar, ac, cells, cnt);
	bool valid = CanPlace(playerBoard, si, rot, ar, ac);

	canvas->SetAlpha(170);
	if (valid) canvas->SetColor(50, 210, 80);
	else canvas->SetColor(210, 50, 50);

	for (int i = 0; i < cnt; i++) {
		int r = cells[i][0], c = cells[i][1];
		if (r < 0 || r >= BOARD || c < 0 || c >= BOARD) continue;
		int px = bx + c * cellSize + 1;
		int py = by + r * cellSize + 1;
		canvas->PutRect(px, py, px + cellSize - 3, py + cellSize - 3, true);
	}
	canvas->SetAlpha(255);
}

void WarPuzzle::DrawShipList(Canvas* canvas, int x, int y, int availH) {
	// Draw remaining ships list on the right panel during placement
	int slotH = max(12, availH / (SHIP_COUNT + 2));
	int dotSz = max(4, min(cellSize / 2, slotH - 4));

	for (int si = 0; si < SHIP_COUNT; si++) {
		bool placed = (playerBoard.ships[si].cellCount > 0);
		bool current = (si == placeIdx);

		int sy = y + si * slotH;
		int cols = SHAPES[si][1]; // use horizontal display (always)
		int rows = SHAPES[si][0];

		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				int px = x + c * (dotSz + 2);
				int py = sy + r * (dotSz + 2);
				if (placed) {
					canvas->SetColor(60, 60, 70);
				} else if (current) {
					canvas->SetColor(255, 220, 50);
				} else {
					canvas->SetColor(50, 100, 190);
				}
				canvas->SetAlpha(255);
				canvas->PutRect(px, py, px + dotSz - 1, py + dotSz - 1, true);
			}
		}
	}
}

void WarPuzzle::DrawScene(Canvas* canvas) {
	// Background
	canvas->SetColor(8, 14, 28);
	canvas->SetAlpha(255);
	canvas->PutRect(0, 0, canvasW - 1, canvasH - 1, true);

	int boardPx = cellSize * BOARD;
	int infoY = max(4, (playerBoardY - 20) / 2);
	int sc = max(1, cellSize / 6);

	if (phase == PHASE_PLACE) {
		// Player board on left
		DrawBoard(canvas, playerBoard, playerBoardX, playerBoardY, false, false, -1, -1);

		// Ghost for current ship
		if (placeIdx < SHIP_COUNT) {
			DrawGhost(canvas, playerBoardX, playerBoardY, placeIdx, placeRot, placeCurR, placeCurC);

			// Cursor border
			int rows = (placeRot == 0) ? SHAPES[placeIdx][0] : SHAPES[placeIdx][1];
			int cols = (placeRot == 0) ? SHAPES[placeIdx][1] : SHAPES[placeIdx][0];
			int px = playerBoardX + placeCurC * cellSize;
			int py = playerBoardY + placeCurR * cellSize;
			canvas->SetColor(255, 255, 255);
			canvas->SetAlpha(80);
			canvas->PutRect(px, py, px + cols * cellSize, py + rows * cellSize, false);
			canvas->SetAlpha(255);
		}

		// Ship list on the right (where AI board will be)
		DrawShipList(canvas, aiBoardX, aiBoardY, boardPx);

		// Status bar - how many placed
		canvas->SetColor(100, 180, 255);
		canvas->SetAlpha(255);
		DrawNumber(canvas, placeIdx, playerBoardX, infoY, sc + 1);
		canvas->SetColor(60, 60, 80);
		int barW = boardPx;
		canvas->PutRect(playerBoardX, infoY + (sc + 1) * 6 + 2,
						playerBoardX + barW - 1, infoY + (sc + 1) * 6 + 5, true);
		canvas->SetColor(100, 180, 255);
		canvas->PutRect(playerBoardX, infoY + (sc + 1) * 6 + 2,
						playerBoardX + barW * placeIdx / SHIP_COUNT - 1,
						infoY + (sc + 1) * 6 + 5, true);

	} else if (phase == PHASE_BATTLE) {
		DrawBoard(canvas, playerBoard, playerBoardX, playerBoardY, false, false, -1, -1);
		DrawBoard(canvas, aiBoard, aiBoardX, aiBoardY, true,
				  playerTurn, playerTurn ? cursorR : -1, playerTurn ? cursorC : -1);

		// Turn indicator bar at top
		if (playerTurn) {
			canvas->SetColor(50, 120, 220);
		} else {
			canvas->SetColor(220, 50, 50);
		}
		canvas->SetAlpha(200);
		int barH = max(4, infoY / 3);
		canvas->PutRect(0, infoY, canvasW - 1, infoY + barH, true);
		canvas->SetAlpha(255);

		// Shot counter (player shots) near top
		canvas->SetColor(200, 200, 200);
		DrawNumber(canvas, playerShots, playerBoardX, infoY - (sc + 1) * 6 - 2, sc);

	} else { // PHASE_END
		DrawBoard(canvas, playerBoard, playerBoardX, playerBoardY, false, false, -1, -1);
		DrawBoard(canvas, aiBoard, aiBoardX, aiBoardY, false, false, -1, -1);

		// Overlay
		if (playerWon) {
			canvas->SetColor(20, 180, 50);
		} else {
			canvas->SetColor(180, 20, 20);
		}
		canvas->SetAlpha(120);
		canvas->PutRect(0, 0, canvasW - 1, canvasH - 1, true);
		canvas->SetAlpha(255);

		// Score (shots taken)
		int numSc = max(3, cellSize / 4);
		canvas->SetColor(255, 230, 60);
		DrawNumber(canvas, playerShots, canvasW / 2 - numSc * 8, canvasH / 2 - numSc * 3, numSc);

		// Win/lose colored marker above score
		if (playerWon) {
			canvas->SetColor(50, 230, 80);
		} else {
			canvas->SetColor(230, 50, 50);
		}
		int mSz = max(6, cellSize / 2);
		canvas->PutRect(canvasW / 2 - mSz, canvasH / 2 - numSc * 3 - mSz * 2 - 4,
						canvasW / 2 + mSz - 1, canvasH / 2 - numSc * 3 - 4, true);
	}
}

// --- main loop ---

int WarPuzzle::Loop(Canvas* canvas, int ms) {
	if (!canvas) return 0;

	if (canvas->GetWidth() != canvasW || canvas->GetHeight() != canvasH) {
		canvasW = canvas->GetWidth();
		canvasH = canvas->GetHeight();
		ComputeLayout();
	}

	float dt = ms / 1000.0f;

	if (phase == PHASE_END) {
		int key;
		while ((key = canvas->BiosKey(0)) != 0) {
			if (!(key & 0x8000)) {
				DrawScene(canvas);
				return playerWon ? 1 : -1;
			}
		}
		DrawScene(canvas);
		return 0;
	}

	if (phase == PHASE_PLACE) {
		int key;
		while ((key = canvas->BiosKey(0)) != 0) {
			if (key & 0x8000) continue; // ignore key-up
			int code = key & ~0x8000;

			int nr = (placeRot == 0) ? SHAPES[placeIdx][0] : SHAPES[placeIdx][1];
			int nc = (placeRot == 0) ? SHAPES[placeIdx][1] : SHAPES[placeIdx][0];

			if (code == canvas->KeyCode("up"))
				placeCurR = max(0, placeCurR - 1);
			else if (code == canvas->KeyCode("down"))
				placeCurR = min(BOARD - nr, placeCurR + 1);
			else if (code == canvas->KeyCode("left"))
				placeCurC = max(0, placeCurC - 1);
			else if (code == canvas->KeyCode("right"))
				placeCurC = min(BOARD - nc, placeCurC + 1);
			else if (code == canvas->KeyCode("r")) {
				placeRot ^= 1;
				int nr2 = (placeRot == 0) ? SHAPES[placeIdx][0] : SHAPES[placeIdx][1];
				int nc2 = (placeRot == 0) ? SHAPES[placeIdx][1] : SHAPES[placeIdx][0];
				placeCurR = min(placeCurR, BOARD - nr2);
				placeCurC = min(placeCurC, BOARD - nc2);
			}
			else if (code == canvas->KeyCode("space") || code == canvas->KeyCode("enter")) {
				if (CanPlace(playerBoard, placeIdx, placeRot, placeCurR, placeCurC)) {
					PlaceShip(playerBoard, placeIdx, placeRot, placeCurR, placeCurC);
					placeIdx++;
					if (placeIdx >= SHIP_COUNT) {
						phase = PHASE_BATTLE;
						playerTurn = true;
						cursorR = 0;
						cursorC = 0;
					} else {
						placeCurR = 0; placeCurC = 0; placeRot = 0;
					}
				}
			}
		}
		DrawScene(canvas);
		return 0;
	}

	// PHASE_BATTLE
	if (playerTurn) {
		int key;
		while ((key = canvas->BiosKey(0)) != 0) {
			if (key & 0x8000) continue;
			int code = key & ~0x8000;

			if (code == canvas->KeyCode("up"))
				cursorR = max(0, cursorR - 1);
			else if (code == canvas->KeyCode("down"))
				cursorR = min(BOARD - 1, cursorR + 1);
			else if (code == canvas->KeyCode("left"))
				cursorC = max(0, cursorC - 1);
			else if (code == canvas->KeyCode("right"))
				cursorC = min(BOARD - 1, cursorC + 1);
			else if (code == canvas->KeyCode("space") || code == canvas->KeyCode("enter")) {
				if (!aiBoard.hit[cursorR][cursorC]) {
					playerShots++;
					bool hit = FireAt(aiBoard, cursorR, cursorC);
					if (AllSunk(aiBoard)) {
						playerWon = true;
						phase = PHASE_END;
					} else if (!hit) {
						playerTurn = false;
						aiDelay = 0.8f;
					}
					// hit: player keeps turn
				}
			}
		}
	} else {
		aiDelay -= dt;
		if (aiDelay <= 0.0f) {
			AIFire();
		}
	}

	DrawScene(canvas);
	return 0;
}
