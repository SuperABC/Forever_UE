#include "phone.h"

#include "player/player.h"

#include <algorithm>


using namespace std;

static const int GRID_COLS = 4;
static const int CELL_W = 120;
static const int CELL_H = 90;
static const int ICON_W = 60;
static const int ICON_H = 60;
static const int GRID_START_Y = 50;

static const int TASK_COLS = 2;
static const int THUMB_W = 120;
static const int THUMB_H = 160;
static const int TASK_CELL_W = 240;
static const int TASK_CELL_H = 200;
static const int TASK_START_Y = 50;


Phone::Phone() {

}

Phone::~Phone() {

}

void Phone::Init(int width, int height) {
	W = width;
	H = height;
	state = HOME;
	homeSelection = 0;
	taskSelection = 0;
	entries.clear();
	currentAppType.clear();
	BuildAppList();
}

void Phone::BuildAppList() {
	appTypes.clear();
	auto enables = Config::GetEnables("app");
	for (auto& type : enables) {
		if (Player::appFactory->CheckRegistered(type)) {
			appTypes.push_back(type);
		}
	}
	homeSelection = 0;
}

int Phone::FindEntry(const string& type) const {
	for (int i = 0; i < (int)entries.size(); i++) {
		if (entries[i].type == type) return i;
	}
	return -1;
}

void Phone::OpenApp(Canvas* canvas, int entryIdx) {
	if (entryIdx < 0 || entryIdx >= (int)entries.size()) return;
	currentAppType = entries[entryIdx].type;
	state = IN_APP;

	if (!entries[entryIdx].initialized) {
		canvas->ClearBuffer();
		Player::appFactory->InitApp(entries[entryIdx].type);
		entries[entryIdx].initialized = true;
	} else {
		Player::appFactory->RefreshApp(entries[entryIdx].type, canvas);
	}
}

void Phone::SaveSnapshot(Canvas* canvas, int entryIdx) {
	if (entryIdx < 0 || entryIdx >= (int)entries.size()) return;
	const uint8_t* data = canvas->GetData();
	int size = canvas->GetDataSize();
	entries[entryIdx].snapshot.assign(data, data + size);
}

void Phone::HandleHomeKeys(Canvas* canvas) {
	int n = (int)appTypes.size();
	int upCode = canvas->KeyCode("up");
	int downCode = canvas->KeyCode("down");
	int leftCode = canvas->KeyCode("left");
	int rightCode = canvas->KeyCode("right");
	int enterCode = canvas->KeyCode("enter");
	int tabCode = canvas->KeyCode("tab");

	while (canvas->BiosKey(1)) {
		int key = canvas->BiosKey(0);
		if (key & 0x8000) continue;

		if (key == rightCode && n > 0) {
			homeSelection = (homeSelection + 1) % n;
		} else if (key == leftCode && n > 0) {
			homeSelection = (homeSelection - 1 + n) % n;
		} else if (key == downCode && n > 0) {
			homeSelection = min(homeSelection + GRID_COLS, n - 1);
		} else if (key == upCode && n > 0) {
			homeSelection = max(homeSelection - GRID_COLS, 0);
		} else if (key == enterCode && n > 0) {
			string type = appTypes[homeSelection];
			int idx = FindEntry(type);
			if (idx < 0) {
				entries.push_back({ type, false, {} });
				idx = (int)entries.size() - 1;
			}
			OpenApp(canvas, idx);
		} else if (key == tabCode) {
			state = TASKLIST;
			taskSelection = max(0, min(taskSelection, (int)entries.size() - 1));
		}
	}
}

void Phone::HandleInAppKeys(Canvas* canvas) {
	int backCode = canvas->KeyCode("backspace");
	int homeCode = canvas->KeyCode("space");
	int taskCode = canvas->KeyCode("tab");

	int entryIdx = FindEntry(currentAppType);
	bool keepPassthrough = true;
	vector<int> passthrough;

	while (canvas->BiosKey(1)) {
		int key = canvas->BiosKey(0);
		bool isPress = !(key & 0x8000);
		int code = key & ~0x8000;

		if (isPress && code == backCode) {
			if (entryIdx >= 0) {
				Player::appFactory->BackApp(entries[entryIdx].type, canvas);
			}
		} else if (isPress && code == homeCode) {
			if (entryIdx >= 0) SaveSnapshot(canvas, entryIdx);
			state = HOME;
			keepPassthrough = false;
		} else if (isPress && code == taskCode) {
			if (entryIdx >= 0) {
				SaveSnapshot(canvas, entryIdx);
				taskSelection = entryIdx;
			}
			state = TASKLIST;
			keepPassthrough = false;
		} else {
			passthrough.push_back(key);
		}
	}

	if (keepPassthrough) {
		for (int k : passthrough) {
			canvas->PushKey(k);
		}
	}
}

void Phone::HandleTaskKeys(Canvas* canvas) {
	int backCode = canvas->KeyCode("backspace");
	int homeCode = canvas->KeyCode("space");
	int tabCode = canvas->KeyCode("tab");
	int upCode = canvas->KeyCode("up");
	int downCode = canvas->KeyCode("down");
	int enterCode = canvas->KeyCode("enter");

	while (canvas->BiosKey(1)) {
		int key = canvas->BiosKey(0);
		if (key & 0x8000) continue;

		int n = (int)entries.size();

		if (key == upCode && n > 0) {
			taskSelection = max(taskSelection - 1, 0);
		} else if (key == downCode && n > 0) {
			taskSelection = min(taskSelection + 1, n - 1);
		} else if (key == enterCode && n > 0) {
			taskSelection = min(taskSelection, n - 1);
			OpenApp(canvas, taskSelection);
		} else if (key == backCode && n > 0) {
			taskSelection = min(taskSelection, n - 1);
			if (entries[taskSelection].type == currentAppType) {
				currentAppType.clear();
			}
			entries.erase(entries.begin() + taskSelection);
			n = (int)entries.size();
			if (n == 0) {
				taskSelection = 0;
				state = HOME;
			} else {
				taskSelection = min(taskSelection, n - 1);
			}
		} else if (key == homeCode || key == tabCode) {
			state = HOME;
		}
	}
}

void Phone::RenderHome(Canvas* canvas) {
	canvas->ClearBuffer();
	canvas->SetAlpha(255);

	canvas->SetColor(20, 20, 40);
	canvas->PutRect(0, 0, W - 1, BAR_Y - 1, true);

	canvas->SetColor(200, 200, 220);
	canvas->SetFontSize(24);
	canvas->PutString("桌面", 216, 10);

	for (int i = 0; i < (int)appTypes.size(); i++) {
		int col = i % GRID_COLS;
		int row = i / GRID_COLS;
		int x = col * CELL_W + (CELL_W - ICON_W) / 2;
		int y = GRID_START_Y + row * CELL_H;

		if (y + CELL_H > BAR_Y) break;

		bool sel = (i == homeSelection);

		if (sel) {
			canvas->SetColor(60, 140, 255);
			canvas->PutRect(x - 3, y - 3, x + ICON_W + 3, y + ICON_H + 3, true);
		}

		canvas->SetColor(220, 220, 220);
		canvas->PutRect(x, y, x + ICON_W, y + ICON_H, true);

		canvas->SetColor(200, 200, 220);
		canvas->SetFontSize(20);
		canvas->PutString(appTypes[i], x, y + ICON_H + 3);
	}

	RenderBottomBar(canvas);
}

void Phone::RenderTaskList(Canvas* canvas) {
	canvas->ClearBuffer();
	canvas->SetAlpha(255);

	canvas->SetColor(15, 15, 25);
	canvas->PutRect(0, 0, W - 1, BAR_Y - 1, true);

	canvas->SetColor(200, 200, 220);
	canvas->SetFontSize(24);
	canvas->PutString("任务列表", 192, 10);

	if (entries.empty()) {
		canvas->SetColor(100, 100, 130);
		canvas->SetFontSize(24);
		canvas->PutString("无运行中的应用", 160, 280);
	} else {
		for (int i = 0; i < (int)entries.size(); i++) {
			int col = i % TASK_COLS;
			int row = i / TASK_COLS;
			int cellY = TASK_START_Y + row * TASK_CELL_H;

			if (cellY + TASK_CELL_H > BAR_Y) break;

			int thumbX = col * TASK_CELL_W + (TASK_CELL_W - THUMB_W) / 2;
			int thumbY = cellY + 10;

			if (!entries[i].snapshot.empty()) {
				canvas->PutRawImage(entries[i].snapshot.data(), W, H,
					thumbX, thumbY, THUMB_W, THUMB_H);
			} else {
				canvas->SetColor(50, 50, 70);
				canvas->PutRect(thumbX, thumbY, thumbX + THUMB_W, thumbY + THUMB_H, true);
			}

			if (i == taskSelection) {
				canvas->SetColor(60, 140, 255);
				canvas->PutRect(thumbX - 2, thumbY - 2,
					thumbX + THUMB_W + 2, thumbY + THUMB_H + 2, false);
			}

			canvas->SetColor(180, 180, 200);
			canvas->SetFontSize(12);
			canvas->PutString(entries[i].type, thumbX, thumbY + THUMB_H + 4);
		}
	}

	RenderBottomBar(canvas);
}

void Phone::RenderBottomBar(Canvas* canvas) {
	canvas->SetAlpha(255);
	canvas->SetColor(25, 25, 35);
	canvas->PutRect(0, BAR_Y, W - 1, H - 1, true);

	canvas->SetColor(160, 160, 170);

	// Back: "<"
	canvas->SetFontSize(26);
	canvas->PutString("<", 66, BAR_Y + 7);

	// Home: circle
	canvas->PutCircle(240, BAR_Y + 20, 13, false);

	// Task: two overlapping squares
	canvas->PutRect(386, BAR_Y + 11, 402, BAR_Y + 27, false);
	canvas->PutRect(392, BAR_Y + 6, 408, BAR_Y + 22, false);
}

int Phone::Loop(Canvas* canvas, int ms) {
	if (!canvas) return 0;

	// Phase 1: handle keys (may change state)
	switch (state) {
	case HOME: HandleHomeKeys(canvas); break;
	case IN_APP: HandleInAppKeys(canvas); break;
	case TASKLIST: HandleTaskKeys(canvas); break;
	}

	// Phase 2: render current state
	switch (state) {
	case HOME:
		RenderHome(canvas);
		break;
	case IN_APP: {
		int idx = FindEntry(currentAppType);
		if (idx >= 0) {
			Player::appFactory->LoopApp(entries[idx].type, canvas, ms);
		} else {
			canvas->ClearBuffer();
		}
		RenderBottomBar(canvas);
		break;
	}
	case TASKLIST:
		RenderTaskList(canvas);
		break;
	}

	return 0;
}
