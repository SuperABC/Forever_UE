#pragma once

#include "class.h"
#include "utility.h"
#include "error.h"
#include "config.h"

#include "player/app_mod.h"
#include "player/canvas.h"

#include <string>
#include <vector>


class Phone {
public:
	Phone();
	~Phone();

	void Init(int width, int height);
	int Loop(Canvas* canvas, int ms);

private:
	int W = 480;
	int H = 640;
	int BAR_Y = 600;

	enum State { HOME, IN_APP, TASKLIST };

	struct AppEntry {
		std::string type;
		bool initialized = false;
		std::vector<uint8_t> snapshot;
	};

	std::vector<std::string> appTypes;
	std::vector<AppEntry> entries;

	State state = HOME;
	int homeSelection = 0;
	int taskSelection = 0;
	std::string currentAppType;

	void BuildAppList();
	int FindEntry(const std::string& type) const;
	void OpenApp(Canvas* canvas, int entryIdx);
	void SaveSnapshot(Canvas* canvas, int entryIdx);

	void HandleHomeKeys(Canvas* canvas);
	void HandleInAppKeys(Canvas* canvas);
	void HandleTaskKeys(Canvas* canvas);

	void RenderHome(Canvas* canvas);
	void RenderTaskList(Canvas* canvas);
	void RenderBottomBar(Canvas* canvas);
};
