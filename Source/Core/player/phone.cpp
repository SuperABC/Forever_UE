#include "phone.h"

#include "player/player.h"

#include <algorithm>


using namespace std;

static const int gridCols = 4;
static const int cellWidth = 120;
static const int cellHeight = 90;
static const int iconWidth = 60;
static const int iconHeight = 60;
static const int gridStartY = 50;

static const int taskCols = 2;
static const int thumbWidth = 120;
static const int thumbHeight = 160;
static const int taskCellWidth = 240;
static const int taskCellHeight = 200;
static const int taskStartY = 50;


Phone::Phone() :
	width(480),
	height(640),
	barY(600),
	state(Home),
	homeSelection(0),
	taskSelection(0) {
}

Phone::~Phone() {

}

void Phone::Init(int width, int height) {
	this->width = width;
	this->height = height;
	state = Home;
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

void Phone::OpenApp(Canvas* canvas, int entryIdx, PostHandle* post) {
	if (entryIdx < 0 || entryIdx >= (int)entries.size()) return;
	currentAppType = entries[entryIdx].type;
	state = InApp;

	if (!entries[entryIdx].initialized) {
		canvas->ClearBuffer();
		Player::appFactory->InitApp(entries[entryIdx].type, post);
		entries[entryIdx].initialized = true;
	} else {
		Player::appFactory->RefreshApp(entries[entryIdx].type, canvas, post);
	}
}

void Phone::SaveSnapshot(Canvas* canvas, int entryIdx) {
	if (entryIdx < 0 || entryIdx >= (int)entries.size()) return;
	const uint8_t* data = canvas->GetData();
	int size = canvas->GetDataSize();
	entries[entryIdx].snapshot.assign(data, data + size);
}

void Phone::HandleHomeKeys(Canvas* canvas, PostHandle* post) {
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
			homeSelection = min(homeSelection + gridCols, n - 1);
		} else if (key == upCode && n > 0) {
			homeSelection = max(homeSelection - gridCols, 0);
		} else if (key == enterCode && n > 0) {
			string type = appTypes[homeSelection];
			int idx = FindEntry(type);
			if (idx < 0) {
				entries.push_back({ type, false, {} });
				idx = (int)entries.size() - 1;
			}
			OpenApp(canvas, idx, post);
		} else if (key == tabCode) {
			state = TaskList;
			taskSelection = max(0, min(taskSelection, (int)entries.size() - 1));
		}
	}
}

void Phone::HandleInAppKeys(Canvas* canvas, PostHandle* post) {
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
				Player::appFactory->BackApp(entries[entryIdx].type, canvas, post);
			}
		} else if (isPress && code == homeCode) {
			if (entryIdx >= 0) SaveSnapshot(canvas, entryIdx);
			state = Home;
			keepPassthrough = false;
		} else if (isPress && code == taskCode) {
			if (entryIdx >= 0) {
				SaveSnapshot(canvas, entryIdx);
				taskSelection = entryIdx;
			}
			state = TaskList;
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

void Phone::HandleTaskKeys(Canvas* canvas, PostHandle* post) {
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
			OpenApp(canvas, taskSelection, post);
		} else if (key == backCode && n > 0) {
			taskSelection = min(taskSelection, n - 1);
			if (entries[taskSelection].type == currentAppType) {
				currentAppType.clear();
			}
			entries.erase(entries.begin() + taskSelection);
			n = (int)entries.size();
			if (n == 0) {
				taskSelection = 0;
				state = Home;
			} else {
				taskSelection = min(taskSelection, n - 1);
			}
		} else if (key == homeCode || key == tabCode) {
			state = Home;
		}
	}
}

void Phone::RenderHome(Canvas* canvas) {
	canvas->ClearBuffer();
	canvas->SetAlpha(255);

	canvas->SetColor(20, 20, 40);
	canvas->PutRect(0, 0, width - 1, barY - 1, true);

	canvas->SetColor(200, 200, 220);
	canvas->SetFontSize(24);
	canvas->PutString("桌面", 216, 10);

	for (int i = 0; i < (int)appTypes.size(); i++) {
		int x = i % gridCols;
		int y = i / gridCols;
		int px = x * cellWidth + (cellWidth - iconWidth) / 2;
		int py = gridStartY + y * cellHeight;

		if (py + cellHeight > barY) break;

		bool sel = (i == homeSelection);

		if (sel) {
			canvas->SetColor(60, 140, 255);
			canvas->PutRect(px - 3, py - 3, px + iconWidth + 3, py + iconHeight + 3, true);
		}

		canvas->SetColor(220, 220, 220);
		canvas->PutRect(px, py, px + iconWidth, py + iconHeight, true);

		canvas->SetColor(200, 200, 220);
		canvas->SetFontSize(20);
		canvas->PutString(appTypes[i], px, py + iconHeight + 3);
	}

	RenderBottomBar(canvas);
}

void Phone::RenderTaskList(Canvas* canvas) {
	canvas->ClearBuffer();
	canvas->SetAlpha(255);

	canvas->SetColor(15, 15, 25);
	canvas->PutRect(0, 0, width - 1, barY - 1, true);

	canvas->SetColor(200, 200, 220);
	canvas->SetFontSize(24);
	canvas->PutString("任务列表", 192, 10);

	if (entries.empty()) {
		canvas->SetColor(100, 100, 130);
		canvas->SetFontSize(24);
		canvas->PutString("无运行中的应用", 160, 280);
	} else {
		for (int i = 0; i < (int)entries.size(); i++) {
			int x = i % taskCols;
			int y = i / taskCols;
			int cellY = taskStartY + y * taskCellHeight;

			if (cellY + taskCellHeight > barY) break;

			int thumbX = x * taskCellWidth + (taskCellWidth - thumbWidth) / 2;
			int thumbY = cellY + 10;

			if (!entries[i].snapshot.empty()) {
				canvas->PutRawImage(entries[i].snapshot.data(), width, height,
					thumbX, thumbY, thumbWidth, thumbHeight);
			} else {
				canvas->SetColor(50, 50, 70);
				canvas->PutRect(thumbX, thumbY, thumbX + thumbWidth, thumbY + thumbHeight, true);
			}

			if (i == taskSelection) {
				canvas->SetColor(60, 140, 255);
				canvas->PutRect(thumbX - 2, thumbY - 2,
					thumbX + thumbWidth + 2, thumbY + thumbHeight + 2, false);
			}

			canvas->SetColor(180, 180, 200);
			canvas->SetFontSize(12);
			canvas->PutString(entries[i].type, thumbX, thumbY + thumbHeight + 4);
		}
	}

	RenderBottomBar(canvas);
}

void Phone::RenderBottomBar(Canvas* canvas) {
	canvas->SetAlpha(255);
	canvas->SetColor(25, 25, 35);
	canvas->PutRect(0, barY, width - 1, height - 1, true);

	canvas->SetColor(160, 160, 170);

	// 返回按钮："<"
	canvas->SetFontSize(26);
	canvas->PutString("<", 66, barY + 7);

	// 主页按钮：圆圈
	canvas->PutCircle(240, barY + 20, 13, false);

	// 任务按钮：两个重叠方块
	canvas->PutRect(386, barY + 11, 402, barY + 27, false);
	canvas->PutRect(392, barY + 6, 408, barY + 22, false);
}

int Phone::Loop(Canvas* canvas, int ms, PostHandle* post) {
	if (!canvas) return 0;

	// 第一阶段：处理按键（可能改变当前状态）
	switch (state) {
	case Home: HandleHomeKeys(canvas, post); break;
	case InApp: HandleInAppKeys(canvas, post); break;
	case TaskList: HandleTaskKeys(canvas, post); break;
	}

	// 第二阶段：渲染当前界面
	switch (state) {
	case Home:
		RenderHome(canvas);
		break;
	case InApp: {
		int idx = FindEntry(currentAppType);
		if (idx >= 0) {
			Player::appFactory->LoopApp(entries[idx].type, canvas, ms, post);
		} else {
			canvas->ClearBuffer();
		}
		RenderBottomBar(canvas);
		break;
	}
	case TaskList:
		RenderTaskList(canvas);
		break;
	}

	return 0;
}
