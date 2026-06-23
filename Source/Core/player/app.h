#pragma once

#include "player/app_mod.h"

#include "class.h"


typedef void (*RegisterModAppsFunc)(AppFactory* factory);

class App {
public:
	App() = delete;
	App(AppFactory* factory, const std::string& app);
	~App();

	std::string GetType() const;
	std::string GetName() const;

	void Init() const;
	int Loop(Canvas* canvas, int ms) const;
	void Back(Canvas* canvas) const;
	void Refresh(Canvas* canvas) const;

private:
	OBJECT_HOLDER AppMod* mod;
	AppFactory* factory;
	std::string type;
	std::string name;
};

class EmptyApp : public AppMod {
public:
	EmptyApp();
	virtual ~EmptyApp();

	static const char* GetId();
	virtual const char* GetType() const override;
	virtual const char* GetName() override;

	static void Init();
	static int Loop(Canvas* canvas, int ms);
	static void Back(Canvas* canvas);
	static void Refresh(Canvas* canvas);

private:
	static int count;

	int id;
	std::string name;
};
