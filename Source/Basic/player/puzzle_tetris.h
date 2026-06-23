#pragma once
#include "player/puzzle_mod.h"


class TetrisPuzzle : public PuzzleMod {
public:
	TetrisPuzzle();
	virtual ~TetrisPuzzle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static void Init(int width, int height);
	static int Loop(Canvas* canvas, int ms);

private:
	static const int COLS = 10;
	static const int ROWS = 20;

	// layout — computed in Init() from canvas size
	static int cellSize;
	static int boardX, boardY;
	static int canvasW, canvasH;
	static int digitScale;

	static int board[ROWS][COLS];
	static int currType, currRot, currX, currY;
	static int nextType;
	static int score, level, lines;
	static float gravTimer;
	static int dasDir, dasTimer;
	static bool gameOver, paused, cleared;

	static void SpawnPiece();
	static bool CanPlace(int type, int rot, int px, int py);
	static void LockPiece();
	static int ClearLines();
	static int GhostY();
	static void DrawBoard(Canvas* canvas);
	static void DrawPiece(Canvas* canvas, int type, int rot, int px, int py, uint8_t alpha);
	static void DrawCell(Canvas* canvas, int gx, int gy, int type, uint8_t alpha);
	static void DrawNumber(Canvas* canvas, int num, int x, int y, int scale);
	static void ComputeLayout(int width, int height);
	static int RandType();
	static int GravInterval();
};
