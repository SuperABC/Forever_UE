#include "phone.h"

#include "player/player.h"
#include "json.h"

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
static const int closeBtnSize = 24;

// 底部工具栏三个按钮的命中区域（左上/右下坐标，Y相对于barY的偏移）
static const int backBtnX1 = 40, backBtnX2 = 110;
static const int homeBtnX1 = 210, homeBtnX2 = 270;
static const int tasksBtnX1 = 360, tasksBtnX2 = 430;
static const int bottomBtnY1 = 2, bottomBtnY2 = 38;


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

Phone::BottomButton Phone::HitBottomBar(int mx, int my) const {
	if (my < barY + bottomBtnY1 || my > barY + bottomBtnY2) return BtnNone;
	if (mx >= backBtnX1 && mx <= backBtnX2) return BtnBack;
	if (mx >= homeBtnX1 && mx <= homeBtnX2) return BtnHome;
	if (mx >= tasksBtnX1 && mx <= tasksBtnX2) return BtnTasks;
	return BtnNone;
}

void Phone::HandleBottomBarMouse(Canvas* canvas, PostHandle* post) {
	int mev;
	vector<int> passthrough;

	while ((mev = canvas->BiosMouse(0)) != -1) {
		bool isPress = (mev & 0x8000) == 0;
		int button = mev & ~0x8000;

		if (isPress && button == 0) {
			MouseState mouse = canvas->MouseStatus();
			BottomButton btn = HitBottomBar(mouse.x, mouse.y);

			if (btn == BtnBack) {
				if (state == InApp) {
					int idx = FindEntry(currentAppType);
					if (idx >= 0) {
						Player::appFactory->BackApp(entries[idx].type, canvas, post);
					}
				} else if (state == TaskList) {
					state = Home;
				}
				continue;
			} else if (btn == BtnHome) {
				if (state == InApp) {
					int idx = FindEntry(currentAppType);
					if (idx >= 0) SaveSnapshot(canvas, idx);
					state = Home;
				} else if (state == TaskList) {
					state = Home;
				}
				continue;
			} else if (btn == BtnTasks) {
				if (state == Home) {
					state = TaskList;
					taskSelection = max(0, min(taskSelection, (int)entries.size() - 1));
				} else if (state == InApp) {
					int idx = FindEntry(currentAppType);
					if (idx >= 0) {
						SaveSnapshot(canvas, idx);
						taskSelection = idx;
					}
					state = TaskList;
				}
				continue;
			}
		}

		// 未命中底部工具栏按钮，原样放回队列，留给当前界面/应用自己处理
		passthrough.push_back(mev);
	}

	for (int m : passthrough) {
		canvas->PushMouseButton(m & ~0x8000, (m & 0x8000) == 0);
	}
}

void Phone::HandleHomeMouse(Canvas* canvas, PostHandle* post) {
	int n = (int)appTypes.size();
	if (n <= 0) return;

	MouseState mouse = canvas->MouseStatus();

	int hoverIdx = -1;
	for (int i = 0; i < n; i++) {
		int x = i % gridCols;
		int y = i / gridCols;
		int px = x * cellWidth + (cellWidth - iconWidth) / 2;
		int py = gridStartY + y * cellHeight;

		if (py + cellHeight > barY) break;

		if (mouse.x >= px - 3 && mouse.x <= px + iconWidth + 3 &&
			mouse.y >= py - 3 && mouse.y <= py + iconHeight + 3) {
			hoverIdx = i;
			break;
		}
	}

	// 鼠标悬停时，选择框直接跟随鼠标，抢占键盘的选择状态
	if (hoverIdx >= 0) homeSelection = hoverIdx;

	int mev;
	while ((mev = canvas->BiosMouse(0)) != -1) {
		if ((mev & 0x8000) != 0 || (mev & ~0x8000) != 0) continue;
		if (hoverIdx < 0) continue;

		string type = appTypes[hoverIdx];
		int idx = FindEntry(type);
		if (idx < 0) {
			entries.push_back({ type, false, {} });
			idx = (int)entries.size() - 1;
		}
		OpenApp(canvas, idx, post);
	}
}

void Phone::HandleTaskMouse(Canvas* canvas, PostHandle* post) {
	int n = (int)entries.size();
	if (n <= 0) return;

	MouseState mouse = canvas->MouseStatus();

	int hoverIdx = -1;
	bool hoverClose = false;
	for (int i = 0; i < n; i++) {
		int x = i % taskCols;
		int y = i / taskCols;
		int cellY = taskStartY + y * taskCellHeight;

		if (cellY + taskCellHeight > barY) break;

		int thumbX = x * taskCellWidth + (taskCellWidth - thumbWidth) / 2;
		int thumbY = cellY + 10;

		if (mouse.x >= thumbX - 2 && mouse.x <= thumbX + thumbWidth + 2 &&
			mouse.y >= thumbY - 2 && mouse.y <= thumbY + thumbHeight + 2) {
			hoverIdx = i;

			int closeX1 = thumbX + thumbWidth - closeBtnSize - 2;
			int closeY1 = thumbY + 2;
			int closeX2 = thumbX + thumbWidth - 2;
			int closeY2 = thumbY + 2 + closeBtnSize;
			hoverClose = (mouse.x >= closeX1 && mouse.x <= closeX2 && mouse.y >= closeY1 && mouse.y <= closeY2);
			break;
		}
	}

	// 鼠标悬停时，选择框直接跟随鼠标，抢占键盘的选择状态
	if (hoverIdx >= 0) taskSelection = hoverIdx;

	int mev;
	while ((mev = canvas->BiosMouse(0)) != -1) {
		if ((mev & 0x8000) != 0 || (mev & ~0x8000) != 0) continue;
		if (hoverIdx < 0) continue;

		if (hoverClose) {
			if (entries[hoverIdx].type == currentAppType) {
				currentAppType.clear();
			}
			entries.erase(entries.begin() + hoverIdx);
			int newN = (int)entries.size();
			if (newN == 0) {
				taskSelection = 0;
				state = Home;
			} else {
				taskSelection = min(taskSelection, newN - 1);
			}
			hoverIdx = -1;
		} else {
			taskSelection = min(hoverIdx, (int)entries.size() - 1);
			OpenApp(canvas, taskSelection, post);
		}
	}
}

bool Phone::GetGameDateTime(PostHandle* post, string& date, string& time) const {
	if (!post) return false;

	JsonValue req(DATA_OBJECT);
	req["post"] = "game time";
	post->Post(req);

	const JsonValue& result = post->GetResult();
	if (result["result"].AsString() != "success") return false;

	date = result["date"].AsString();
	time = result["time"].AsString();
	return true;
}

void Phone::RenderHome(Canvas* canvas, PostHandle* post) {
	canvas->ClearBuffer();
	canvas->SetAlpha(255);

	canvas->SetColor(20, 20, 40);
	canvas->PutRect(0, 0, width - 1, barY - 1, true);

	canvas->SetColor(200, 200, 220);
	canvas->SetFontSize(24);
	canvas->PutString("桌面", 216, 10);

	string gameDate, gameTime;
	if (GetGameDateTime(post, gameDate, gameTime)) {
		canvas->SetColor(200, 200, 220);
		canvas->SetFontSize(24);

		// 左上角显示日期
		canvas->PutString(gameDate, 12, 10);

		// 右上角显示时间（只精确到分钟）
		int timeWidth = canvas->MeasureString(gameTime);
		canvas->PutString(gameTime, width - timeWidth - 12, 10);
	}

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

			// 关闭按钮："×"，位于缩略图右上角
			int closeX1 = thumbX + thumbWidth - closeBtnSize - 2;
			int closeY1 = thumbY + 2;
			canvas->SetColor(220, 90, 90);
			canvas->SetFontSize(22);
			canvas->PutString("×", closeX1 + 3, closeY1 + 1);
		}
	}

	RenderBottomBar(canvas);
}

void Phone::RenderBottomBar(Canvas* canvas) {
	canvas->SetAlpha(255);
	canvas->SetColor(25, 25, 35);
	canvas->PutRect(0, barY, width - 1, height - 1, true);

	MouseState mouse = canvas->MouseStatus();
	BottomButton hover = HitBottomBar(mouse.x, mouse.y);

	if (hover != BtnNone) {
		canvas->SetColor(60, 140, 255);
		if (hover == BtnBack) {
			canvas->PutRect(backBtnX1, barY + bottomBtnY1, backBtnX2, barY + bottomBtnY2, false);
		} else if (hover == BtnHome) {
			canvas->PutRect(homeBtnX1, barY + bottomBtnY1, homeBtnX2, barY + bottomBtnY2, false);
		} else if (hover == BtnTasks) {
			canvas->PutRect(tasksBtnX1, barY + bottomBtnY1, tasksBtnX2, barY + bottomBtnY2, false);
		}
	}

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

	// 第一阶段：处理按键与鼠标（可能改变当前状态）
	switch (state) {
	case Home:
		HandleHomeKeys(canvas, post);
		HandleBottomBarMouse(canvas, post);
		HandleHomeMouse(canvas, post);
		break;
	case InApp:
		HandleInAppKeys(canvas, post);
		HandleBottomBarMouse(canvas, post);
		break;
	case TaskList:
		HandleTaskKeys(canvas, post);
		HandleBottomBarMouse(canvas, post);
		HandleTaskMouse(canvas, post);
		break;
	}

	// 第二阶段：渲染当前界面
	switch (state) {
	case Home:
		RenderHome(canvas, post);
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
