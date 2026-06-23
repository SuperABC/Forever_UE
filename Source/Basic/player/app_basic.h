#pragma once

#include "player/app_mod.h"


class TestApp : public AppMod {
public:
	TestApp();
	virtual ~TestApp();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static void Init();
	static int Loop(Canvas* canvas, int ms);
	static void Back(Canvas* canvas);
	static void Refresh(Canvas* canvas);

private:
	static int count;
	static float posX, posY;
	static float velX, velY;

	int id;
	std::string name;
};
