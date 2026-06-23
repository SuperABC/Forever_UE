#pragma once
#include "player/puzzle_mod.h"


class FlappyPuzzle : public PuzzleMod {
public:
	FlappyPuzzle();
	virtual ~FlappyPuzzle();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static void Init(int width, int height);
	static int Loop(Canvas* canvas, int ms);

private:
	static const int MAX_PIPES = 6;

	struct Pipe {
		float x, gapY, gapH, w;
		bool passed;
	};

	static int canvasW, canvasH;
	static float birdX, birdY, birdVY, birdR;
	static float hoverT;

	static Pipe pipes[MAX_PIPES];
	static int pipeCount;
	static float pipeTimer;
	static int score;
	static int phase; // 0=wait 1=play 2=dead

	// physics / layout (computed from canvas size)
	static float gravity, flapVel, pipeSpeed, pipeInterval;
	static float pipeGap, pipeW, groundH;

	static void ComputeLayout();
	static void SpawnPipe();
	static bool CircleRect(float cx, float cy, float cr,
						   float rx1, float ry1, float rx2, float ry2);
	static bool Collides();
	static void DrawNumber(Canvas* canvas, int num, int x, int y, int scale);
	static void DrawScene(Canvas* canvas);
};
