#include "player.h"

#include "player/puzzle.h"
#include "player/app.h"


using namespace std;

PuzzleFactory* Player::puzzleFactory = nullptr;
AppFactory* Player::appFactory = nullptr;

Player::Player() :
	time(nullptr),
	day(-1),
	phone(nullptr) {
	if (!puzzleFactory) {
		puzzleFactory = new PuzzleFactory();
	}
	if (!appFactory) {
		appFactory = new AppFactory();
	}
}

Player::~Player() {
	Destroy();
}

void Player::LoadConfigs() const {
	puzzleFactory->RemoveAll();
	auto puzzles = Config::GetEnables("puzzle");
	for (auto puzzle : puzzles) {
		puzzleFactory->SetConfig(puzzle, true);
	}

	appFactory->RemoveAll();
	auto apps = Config::GetEnables("app");
	for (auto app : apps) {
		appFactory->SetConfig(app, true);
	}
}

void Player::InitPuzzles(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	puzzleFactory->RegisterPuzzle(EmptyPuzzle::GetId(),
		EmptyPuzzle::Init,
		EmptyPuzzle::Loop,
		[]() { return new EmptyPuzzle(); },
		[](PuzzleMod* puzzle) { delete puzzle; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			debugf("Log: %s loaded successfully.\n", dll.data());

			auto registerFunc = reinterpret_cast<RegisterModPuzzlesFunc>(GetProcAddress(modHandle, "RegisterModPuzzles"));
			if (registerFunc) {
				registerFunc(puzzleFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Player::InitApps(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {
	appFactory->RegisterApp(EmptyApp::GetId(),
		EmptyApp::Init,
		EmptyApp::Loop,
		EmptyApp::Back,
		EmptyApp::Refresh,
		[]() { return new EmptyApp(); },
		[](AppMod* app) { delete app; }
	);

	for (auto dll : dlls) {
		HMODULE modHandle;
		if (modHandles.find(dll) != modHandles.end()) {
			modHandle = modHandles[dll];
		}
		else {
			modHandle = LoadLibraryA(dll.data());
			modHandles[dll] = modHandle;
		}
		if (modHandle) {
			auto registerFunc = reinterpret_cast<RegisterModAppsFunc>(GetProcAddress(modHandle, "RegisterModApps"));
			if (registerFunc) {
				registerFunc(appFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
	}
}

void Player::Init() {
	Destroy();

	time = new Time();
	time->SetHour(8);

	phone = new Phone();
	phone->Init(480, 640);
}

void Player::Destroy() {
	delete time;
	time = nullptr;
	delete phone;
	phone = nullptr;
}

void Player::Tick(float delta) {
	day = time->GetDay();
	time->AddMilliseconds((int)(delta * 60 * 1000));
}

void Player::ApplyChange(Change* change,
	const vector<function<pair<bool, ValueType>(const string&)>>& getValues) {

}

Time* Player::GetTime() {
	return time;
}

bool Player::CrossDay() {
	return day == time->GetDay();
}

Phone* Player::GetPhone() const {
	return phone;
}
