#include "app_zheye.h"
#include "player/canvas.h"
#include "json.h"

#include <filesystem>
#include <fstream>
#include <algorithm>


using namespace std;
namespace fs = std::filesystem;

static const int TAB_BAR_HEIGHT = 52;
static const int TAB_PADDING = 20;
static const int TAB_ARROW_WIDTH = 28;
static const int LIST_TOP = TAB_BAR_HEIGHT;
static const int LIST_BOTTOM = 600;
static const int ROW_HEIGHT = 60;
static const int DETAIL_BOTTOM = 600;
static const int CONTENT_MARGIN = 12;
static const int CONTENT_WIDTH = 480 - CONTENT_MARGIN * 2;

string ZheyeApp::publicPath = "";
bool ZheyeApp::available = false;
bool ZheyeApp::runtimeSaveReady = false;

vector<ZheyeApp::Section> ZheyeApp::sections;
int ZheyeApp::currentSection = 0;
int ZheyeApp::selectedPost = 0;
int ZheyeApp::listScrollTop = 0;
int ZheyeApp::tabScrollX = 0;

ZheyeApp::Screen ZheyeApp::screen = ZheyeApp::SectionList;
int ZheyeApp::openSectionIndex = -1;
int ZheyeApp::openPostIndex = -1;
int ZheyeApp::replyScrollTop = -1;
bool ZheyeApp::detailOverflowed = false;

vector<ZheyeApp::RowRect> ZheyeApp::rowRects;
vector<ZheyeApp::TabRect> ZheyeApp::tabRects;

ZheyeApp::ZheyeApp() {

}

ZheyeApp::~ZheyeApp() {

}

const char* ZheyeApp::GetId() {
	return "zheye";
}

const char* ZheyeApp::GetType() const {
	return "zheye";
}

const char* ZheyeApp::GetName() {
	name = "者也";
	return name.data();
}

bool ZheyeApp::ReadJsonFile(const string& path, JsonValue& out) {
	if (!fs::exists(path)) return false;

	ifstream fin(path);
	if (!fin.is_open()) return false;

	JsonReader reader;
	bool ok = reader.Parse(fin, out);
	fin.close();
	return ok;
}

void ZheyeApp::ResetRuntimeSave() {
	error_code ec;
	fs::copy_file(fs::path(publicPath) / "data.json", fs::path(publicPath) / "runtime.json",
		fs::copy_options::overwrite_existing, ec);
}

bool ZheyeApp::LoadData() {
	sections.clear();

	JsonValue root;
	if (!ReadJsonFile((fs::path(publicPath) / "runtime.json").string(), root)) return false;

	for (auto& sectionVal : root["sections"]) {
		Section section;
		section.name = sectionVal["name"].AsString();

		for (auto& postVal : sectionVal["posts"]) {
			Post post;
			post.title = postVal["title"].AsString();
			post.author = postVal["author"].AsString();
			post.content = postVal["content"].AsString();

			for (auto& replyVal : postVal["replies"]) {
				Reply reply;
				reply.author = replyVal["author"].AsString();
				reply.content = replyVal["content"].AsString();
				post.replies.push_back(reply);
			}

			section.posts.push_back(post);
		}

		sections.push_back(section);
	}

	return true;
}

void ZheyeApp::OpenPost(int sectionIndex, int postIndex) {
	openSectionIndex = sectionIndex;
	openPostIndex = postIndex;
	replyScrollTop = -1;
	detailOverflowed = false;

	screen = PostDetail;
}

void ZheyeApp::HandleListKeys(Canvas* canvas) {
	int upCode = canvas->KeyCode("up");
	int downCode = canvas->KeyCode("down");
	int leftCode = canvas->KeyCode("left");
	int rightCode = canvas->KeyCode("right");
	int enterCode = canvas->KeyCode("enter");

	while (canvas->BiosKey(1)) {
		int key = canvas->BiosKey(0);
		if (key & 0x8000) continue;

		int n = static_cast<int>(sections.size());
		if (n <= 0) continue;

		if (key == leftCode) {
			if (currentSection > 0) {
				currentSection--;
				selectedPost = 0;
				listScrollTop = 0;
			}
		}
		else if (key == rightCode) {
			if (currentSection < n - 1) {
				currentSection++;
				selectedPost = 0;
				listScrollTop = 0;
			}
		}
		else if (key == upCode) {
			if (!sections[currentSection].posts.empty()) {
				selectedPost = max(selectedPost - 1, 0);
			}
		}
		else if (key == downCode) {
			int total = static_cast<int>(sections[currentSection].posts.size());
			if (total > 0) {
				selectedPost = min(selectedPost + 1, total - 1);
			}
		}
		else if (key == enterCode) {
			auto& section = sections[currentSection];
			if (selectedPost >= 0 && selectedPost < static_cast<int>(section.posts.size())) {
				OpenPost(currentSection, selectedPost);
			}
		}
	}

	int visibleRows = max(1, (LIST_BOTTOM - LIST_TOP) / ROW_HEIGHT);
	if (selectedPost < listScrollTop) listScrollTop = selectedPost;
	else if (selectedPost >= listScrollTop + visibleRows) listScrollTop = selectedPost - visibleRows + 1;
}

void ZheyeApp::HandleListMouse(Canvas* canvas) {
	int mev;
	while ((mev = canvas->BiosMouse(0)) != -1) {
		if ((mev & 0x8000) != 0 || (mev & ~0x8000) != 0) continue;

		MouseState mouse = canvas->MouseStatus();
		for (auto& rect : rowRects) {
			if (mouse.x < rect.x1 || mouse.x > rect.x2 || mouse.y < rect.y1 || mouse.y > rect.y2) continue;

			if (currentSection >= 0 && currentSection < static_cast<int>(sections.size())) {
				auto& section = sections[currentSection];
				if (rect.postIndex >= 0 && rect.postIndex < static_cast<int>(section.posts.size())) {
					selectedPost = rect.postIndex;
					OpenPost(currentSection, rect.postIndex);
				}
			}
			break;
		}
	}
}

void ZheyeApp::HandleTabMouse(Canvas* canvas) {
	int n = static_cast<int>(sections.size());

	int mev;
	vector<int> passthrough;

	while ((mev = canvas->BiosMouse(0)) != -1) {
		bool isPress = (mev & 0x8000) == 0;
		int button = mev & ~0x8000;

		if (isPress && button == 0) {
			MouseState mouse = canvas->MouseStatus();

			if (mouse.y >= 0 && mouse.y < TAB_BAR_HEIGHT) {
				if (mouse.x < TAB_ARROW_WIDTH) {
					if (currentSection > 0) {
						currentSection--;
						selectedPost = 0;
						listScrollTop = 0;
					}
					continue;
				}
				else if (mouse.x >= 480 - TAB_ARROW_WIDTH) {
					if (currentSection < n - 1) {
						currentSection++;
						selectedPost = 0;
						listScrollTop = 0;
					}
					continue;
				}
				else {
					bool hit = false;
					for (auto& rect : tabRects) {
						if (mouse.x < rect.x1 || mouse.x > rect.x2 || mouse.y < rect.y1 || mouse.y > rect.y2) continue;

						if (rect.sectionIndex != currentSection) {
							currentSection = rect.sectionIndex;
							selectedPost = 0;
							listScrollTop = 0;
						}
						hit = true;
						break;
					}
					if (hit) continue;
				}
			}
		}

		// 未命中标签栏，原样放回队列，留给帖子列表自己处理
		passthrough.push_back(mev);
	}

	for (int m : passthrough) {
		canvas->PushMouseButton(m & ~0x8000, (m & 0x8000) == 0);
	}
}

void ZheyeApp::HandleDetailKeys(Canvas* canvas) {
	int upCode = canvas->KeyCode("up");
	int downCode = canvas->KeyCode("down");

	int maxTop = -1;
	if (openSectionIndex >= 0 && openSectionIndex < static_cast<int>(sections.size())) {
		auto& section = sections[openSectionIndex];
		if (openPostIndex >= 0 && openPostIndex < static_cast<int>(section.posts.size())) {
			maxTop = static_cast<int>(section.posts[openPostIndex].replies.size()) - 1;
		}
	}

	while (canvas->BiosKey(1)) {
		int key = canvas->BiosKey(0);
		if (key & 0x8000) continue;

		if (key == upCode) {
			replyScrollTop = max(replyScrollTop - 1, -1);
		}
		else if (key == downCode && detailOverflowed) {
			replyScrollTop = min(replyScrollTop + 1, max(maxTop, -1));
		}
	}

	// 详情页不响应鼠标点击，丢弃期间产生的所有鼠标事件，
	// 避免阅读时的点击残留到返回列表页后被误判成点击某一行
	canvas->ClearMouseBuffer();
}

void ZheyeApp::RenderTabBar(Canvas* canvas) {
	int n = static_cast<int>(sections.size());

	canvas->SetAlpha(255);
	canvas->SetColor(25, 25, 35);
	canvas->PutRect(0, 0, 479, TAB_BAR_HEIGHT - 1, true);

	// 分区标签的可用区域夹在左右两个箭头按钮之间
	int areaX1 = TAB_ARROW_WIDTH;
	int areaX2 = 480 - TAB_ARROW_WIDTH;
	int areaWidth = areaX2 - areaX1;

	tabRects.clear();

	if (n > 0) {
		canvas->SetFontSize(28);

		// 按各分区文字实际宽度顺序排列，而不是等分可用宽度
		vector<int> tabX(n), tabW(n);
		int cursorX = 0;
		for (int i = 0; i < n; i++) {
			tabW[i] = canvas->MeasureString(sections[i].name) + TAB_PADDING * 2;
			tabX[i] = cursorX;
			cursorX += tabW[i];
		}
		int totalWidth = cursorX;

		// 保证当前选中的分区完整显示在可用区域内，超出就水平滚动标签栏
		int selLeft = tabX[currentSection];
		int selRight = tabX[currentSection] + tabW[currentSection];
		if (selLeft < tabScrollX) tabScrollX = selLeft;
		if (selRight > tabScrollX + areaWidth) tabScrollX = selRight - areaWidth;
		tabScrollX = max(0, min(tabScrollX, max(0, totalWidth - areaWidth)));

		for (int i = 0; i < n; i++) {
			int tx = areaX1 + tabX[i] - tabScrollX;
			if (tx + tabW[i] < areaX1 || tx > areaX2) continue;

			bool sel = (i == currentSection);
			if (sel) canvas->SetColor(230, 230, 235);
			else canvas->SetColor(160, 160, 170);
			canvas->PutString(sections[i].name, tx + TAB_PADDING, 8);

			if (sel) {
				canvas->SetColor(60, 140, 255);
				canvas->PutRect(tx, TAB_BAR_HEIGHT - 3, tx + tabW[i] - 1, TAB_BAR_HEIGHT - 1, true);
			}

			tabRects.push_back({ tx, 0, tx + tabW[i] - 1, TAB_BAR_HEIGHT - 1, i });
		}
	}

	// 左右切换箭头，到达边界时变暗表示不可再切换
	bool canLeft = currentSection > 0;
	bool canRight = n > 0 && currentSection < n - 1;
	int midY = TAB_BAR_HEIGHT / 2;

	canvas->SetColor(canLeft ? 210 : 70, canLeft ? 210 : 70, canLeft ? 220 : 80);
	canvas->PutTriangle(20, midY - 8, 20, midY + 8, 8, midY, true);

	canvas->SetColor(canRight ? 210 : 70, canRight ? 210 : 70, canRight ? 220 : 80);
	canvas->PutTriangle(460, midY - 8, 460, midY + 8, 472, midY, true);

	canvas->SetColor(60, 60, 75);
	canvas->PutLine(0, TAB_BAR_HEIGHT, 479, TAB_BAR_HEIGHT);
}

void ZheyeApp::RenderEmptyState(Canvas* canvas, const string& msg) {
	canvas->SetColor(120, 120, 140);
	canvas->SetFontSize(26);
	canvas->PutString(msg, 16, 300);
}

void ZheyeApp::RenderSectionList(Canvas* canvas) {
	canvas->ClearBuffer();
	RenderTabBar(canvas);
	rowRects.clear();

	if (sections.empty()) {
		RenderEmptyState(canvas, "暂无版块");
		return;
	}

	Section& section = sections[currentSection];
	if (section.posts.empty()) {
		canvas->SetColor(120, 120, 140);
		canvas->SetFontSize(24);
		canvas->PutString("暂无帖子", CONTENT_MARGIN, LIST_TOP + 16);
		return;
	}

	int py = LIST_TOP;
	for (int i = listScrollTop; i < static_cast<int>(section.posts.size()); i++) {
		if (py + ROW_HEIGHT > LIST_BOTTOM) break;

		Post& post = section.posts[i];

		bool sel = (i == selectedPost);
		if (sel) {
			canvas->SetAlpha(70);
			canvas->SetColor(60, 140, 255);
			canvas->PutRect(0, py, 479, py + ROW_HEIGHT - 1, true);
			canvas->SetAlpha(255);
		}

		canvas->SetFontSize(24);
		if (sel) canvas->SetColor(230, 230, 235);
		else canvas->SetColor(200, 200, 210);
		canvas->PutString(post.title, CONTENT_MARGIN, py + 6);

		canvas->SetColor(160, 160, 170);
		canvas->SetFontSize(21);
		canvas->PutString(to_string(post.replies.size()) + "回复", 330, py + 32);

		canvas->SetColor(40, 40, 55);
		canvas->PutLine(0, py + ROW_HEIGHT - 1, 479, py + ROW_HEIGHT - 1);

		rowRects.push_back({ 0, py, 479, py + ROW_HEIGHT - 1, i });

		py += ROW_HEIGHT;
	}
}

void ZheyeApp::RenderPostDetail(Canvas* canvas) {
	canvas->ClearBuffer();

	if (openSectionIndex < 0 || openSectionIndex >= static_cast<int>(sections.size())) return;
	Section& section = sections[openSectionIndex];
	if (openPostIndex < 0 || openPostIndex >= static_cast<int>(section.posts.size())) return;
	Post& post = section.posts[openPostIndex];

	int py = 0;

	// 标题和作者固定在顶部，滚动时不移动，只有下面的正文和回复区域会滚动
	canvas->SetColor(230, 230, 235);
	canvas->SetFontSize(30);
	py += 8;
	py += canvas->PutWrappedString(post.title, CONTENT_MARGIN, py, CONTENT_WIDTH);
	py += 10;

	canvas->SetColor(150, 150, 165);
	canvas->SetFontSize(22);
	canvas->PutString("作者: " + post.author, CONTENT_MARGIN, py);
	py += 30;

	canvas->SetColor(70, 70, 90);
	canvas->PutLine(0, py, 479, py);
	py += 8;

	if (replyScrollTop < 0) {
		canvas->SetColor(210, 210, 220);
		canvas->SetFontSize(24);
		py += canvas->PutWrappedString(post.content, CONTENT_MARGIN, py, CONTENT_WIDTH);
		py += 12;

		canvas->SetColor(70, 70, 90);
		canvas->PutLine(0, py, 479, py);
		py += 8;
	}

	int startIdx = max(replyScrollTop, 0);
	for (int i = startIdx; i < static_cast<int>(post.replies.size()); i++) {
		if (py >= DETAIL_BOTTOM) break;

		Reply& reply = post.replies[i];

		canvas->SetColor(140, 180, 255);
		canvas->SetFontSize(22);
		canvas->PutString(to_string(i + 1) + "楼 " + reply.author, CONTENT_MARGIN, py);
		py += 32;

		canvas->SetColor(200, 200, 210);
		canvas->SetFontSize(23);
		py += canvas->PutWrappedString(reply.content, CONTENT_MARGIN, py, CONTENT_WIDTH);
		py += 12;

		canvas->SetColor(50, 50, 65);
		canvas->PutLine(0, py, 479, py);
		py += 8;
	}

	// 只有实际渲染内容超出了可视区域，下滚键才允许继续下移
	detailOverflowed = (py >= DETAIL_BOTTOM);
}

void ZheyeApp::Init(PostHandle* post) {
	available = false;
	screen = SectionList;
	currentSection = 0;
	selectedPost = 0;
	listScrollTop = 0;
	tabScrollX = 0;
	sections.clear();
	rowRects.clear();

	if (!post) return;

	JsonValue req(DATA_OBJECT);
	req["post"] = "config path";
	post->Post(req);

	const JsonValue& result = post->GetResult();
	if (result["result"].AsString() != "success") return;

	publicPath = (fs::path(result["path"].AsString()) / "public" / "zheye").string();

	if (!runtimeSaveReady) {
		ResetRuntimeSave();
		runtimeSaveReady = true;
	}

	available = LoadData();
}

int ZheyeApp::Loop(Canvas* canvas, int ms, PostHandle* post) {
	if (!canvas) return 0;

	if (!available) {
		canvas->ClearBuffer();
		RenderEmptyState(canvas, "数据加载失败");
		return 0;
	}

	if (screen == SectionList) {
		HandleListKeys(canvas);
		HandleTabMouse(canvas);
		HandleListMouse(canvas);
		RenderSectionList(canvas);
	}
	else {
		HandleDetailKeys(canvas);
		RenderPostDetail(canvas);
	}

	return 0;
}

void ZheyeApp::Back(Canvas* canvas, PostHandle* post) {
	if (screen == PostDetail) {
		screen = SectionList;
	}
}

void ZheyeApp::Refresh(Canvas* canvas, PostHandle* post) {
	if (!canvas) return;

	if (!available) {
		canvas->ClearBuffer();
		RenderEmptyState(canvas, "数据加载失败");
		return;
	}

	if (screen == SectionList) RenderSectionList(canvas);
	else RenderPostDetail(canvas);
}
