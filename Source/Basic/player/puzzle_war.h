#pragma once
#include "player/puzzle_mod.h"


class WarPuzzle : public PuzzleMod {
public:
	WarPuzzle();
	virtual ~WarPuzzle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static void Init(int width, int height);
	static int Loop(Canvas* canvas, int ms);

private:
	static const int BOARD = 8;
	static const int SHIP_COUNT = 6;
	static const int MAX_CELLS = 6;

	// {rows, cols} for each of the 6 ships
	static const int SHAPES[SHIP_COUNT][2];

	struct ShipState {
		int cells[MAX_CELLS][2]; // absolute {row,col} on board
		int cellCount;
		bool sunk;
	};

	struct BoardState {
		int grid[BOARD][BOARD]; // -1=empty, >=0=ship index
		bool hit[BOARD][BOARD];
		ShipState ships[SHIP_COUNT];
	};

	// Layout
	static int canvasW, canvasH, cellSize;
	static int playerBoardX, playerBoardY;
	static int aiBoardX, aiBoardY;

	// Game state
	static BoardState playerBoard, aiBoard;

	// Placement
	static int placeIdx;
	static int placeCurR, placeCurC, placeRot;

	// Battle
	static int cursorR, cursorC;
	static bool playerTurn;
	static int phase; // 0=place 1=battle 2=end
	static bool playerWon;
	static float aiDelay;
	static int playerShots;

	static void ComputeLayout();
	static void GetCells(int si, int rot, int ar, int ac, int out[][2], int& cnt);
	static bool CanPlace(BoardState& b, int si, int rot, int ar, int ac);
	static void PlaceShip(BoardState& b, int si, int rot, int ar, int ac);
	static void AIPlace();
	static bool FireAt(BoardState& b, int r, int c);
	static void CheckSunk(BoardState& b, int si);
	static bool AllSunk(const BoardState& b);
	static void AIFire();
	static void DrawBoard(Canvas* canvas, const BoardState& b, int bx, int by,
						  bool hideShips, bool showCursor, int cr, int cc);
	static void DrawGhost(Canvas* canvas, int bx, int by, int si, int rot, int ar, int ac);
	static void DrawShipList(Canvas* canvas, int x, int y, int availH);
	static void DrawNumber(Canvas* canvas, int num, int x, int y, int scale);
	static void DrawScene(Canvas* canvas);
};
