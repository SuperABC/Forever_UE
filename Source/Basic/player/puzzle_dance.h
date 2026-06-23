#pragma once
#include "player/puzzle_mod.h"


class DancePuzzle : public PuzzleMod {
public:
	DancePuzzle();
	virtual ~DancePuzzle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static void Init(int width, int height);
	static int Loop(Canvas* canvas, int ms);

	static const int TOTAL_ROUNDS = 20;
	static const int MAX_ARROWS = 8;

private:
	static int canvasW, canvasH;

	static int round;
	static int arrowCount;
	static int arrows[MAX_ARROWS];
	static int inputPos;
	static bool inputDone;
	static int results[TOTAL_ROUNDS]; // 0=fail 1=pass 3=perfect
	static int totalScore;

	static float elapsed;
	static float roundDuration;
	static int phase; // 0=input 1=result 2=done
	static float phaseTimer;
	static bool wrongFlash;
	static float wrongFlashTimer;

	static void StartRound();
	static void DrawArrow(Canvas* canvas, int type, int cx, int cy, int sz,
						  uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	static void DrawNumber(Canvas* canvas, int num, int x, int y, int scale);
	static void DrawScene(Canvas* canvas);
	static int ArrowCountForRound(int rnd);
};
