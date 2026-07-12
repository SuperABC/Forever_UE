#include "player.h"

#include "player/asset.h"
#include "player/puzzle.h"
#include "player/app.h"


using namespace std;

AssetFactory* Player::assetFactory = nullptr;
PuzzleFactory* Player::puzzleFactory = nullptr;
AppFactory* Player::appFactory = nullptr;

Player::Player() :
	time(nullptr),
	day(-1),
	phone(nullptr),
	deposit(0),
	leftHand(nullptr),
	rightHand(nullptr),
	backPack(nullptr) {
	if (!assetFactory) {
		assetFactory = new AssetFactory();
	}
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
	assetFactory->RemoveAll();
	auto assets = Config::GetEnables("asset");
	for (auto asset : assets) {
		assetFactory->SetConfig(asset, true);
	}

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

void Player::InitAssets(unordered_map<string, HMODULE>& modHandles,
	const vector<string>& dlls) {

	assetFactory->RegisterAsset(EmptyAsset::GetId(),
		[]() { return new EmptyAsset(); },
		[](AssetMod* asset) { delete asset; }
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

			auto registerFunc =
				reinterpret_cast<RegisterModAssetsFunc>(GetProcAddress(modHandle, "RegisterModAssets"));
			if (registerFunc) {
				registerFunc(assetFactory);
			}
		}
		else {
			debugf("Warning: Failed to load %s.\n", dll.data());
		}
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

	leftHand = new Asset(assetFactory, "container");
	leftHand->DefineAsset();
	rightHand = new Asset(assetFactory, "container");
	rightHand->DefineAsset();
	for (int i = 0; i < 10; i++) {
		auto asset = new Asset(assetFactory, "container");
		asset->DefineAsset();
		leftHand->AddContent(asset->GetName(), asset);
	}
	for (int i = 0; i < 10; i++) {
		auto asset = new Asset(assetFactory, "container");
		asset->DefineAsset();
		rightHand->AddContent(asset->GetName(), asset);
	}
}

void Player::Destroy() {
	delete time;
	time = nullptr;
	delete phone;
	phone = nullptr;
	if (leftHand) {
		delete leftHand;
		leftHand = nullptr;
	}
	if (rightHand) {
		delete rightHand;
		rightHand = nullptr;
	}
	if (backPack) {
		delete backPack;
		backPack = nullptr;
	}
	for (auto asset : systemAsset) {
		delete asset;
	}
	systemAsset.clear();
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
	return day != time->GetDay();
}

Phone* Player::GetPhone() const {
	return phone;
}

int Player::GetDeposit() const {
	return deposit;
}

void Player::AddDeposit(int amount) {
	deposit += amount;
}

bool Player::AddSystemAsset(Asset* asset) {
	if (!asset) {
		THROW_EXCEPTION(NullPointerException, "AddSystemAsset: asset is null.\n");
	}
	auto mobility = asset->GetMobility();
	if (mobility != ASSET_ESTATE && mobility != ASSET_VEHICLE) {
		return false;
	}
	if (HasSystemAsset(asset)) {
		return false;
	}
	systemAsset.push_back(asset);
	return true;
}

Asset* Player::TransferSystemAsset(Asset* asset) {
	if (!asset) {
		return nullptr;
	}
	for (auto it = systemAsset.begin(); it != systemAsset.end(); ++it) {
		if (*it == asset) {
			systemAsset.erase(it);
			return asset;
		}
	}
	return nullptr;
}

void Player::DestroySystemAsset(Asset* asset) {
	if (!asset) {
		return;
	}
	for (auto it = systemAsset.begin(); it != systemAsset.end(); ++it) {
		if (*it == asset) {
			systemAsset.erase(it);
			delete asset;
			return;
		}
	}
}

const vector<Asset*>& Player::GetSystemAssets() const {
	return systemAsset;
}

bool Player::HasSystemAsset(Asset* asset) const {
	if (!asset) {
		return false;
	}
	for (auto a : systemAsset) {
		if (a == asset) {
			return true;
		}
	}
	return false;
}

vector<string> Player::SplitPath(const string& path) {
	vector<string> parts;
	string segment;
	for (char character : path) {
		if (character == '/') {
			if (!segment.empty()) {
				parts.push_back(segment);
				segment.clear();
			}
		} else {
			segment += character;
		}
	}
	if (!segment.empty()) {
		parts.push_back(segment);
	}
	return parts;
}

Asset* Player::GetByPath(const string& path) {
	auto parts = SplitPath(path);
	if (parts.empty()) return nullptr;

	Asset* current = nullptr;
	if (parts[0] == "left") {
		current = leftHand;
	}
	else if (parts[0] == "right") {
		current = rightHand;
	}
	else if (parts[0] == "back") {
		current = backPack;
	}
	else if (parts[0] == "room") {
		return nullptr;
	}
	else {
		return nullptr;
	}

	if (parts.size() == 1) return current;

	if (!current || current->GetName() != parts[1]) return nullptr;
	for (size_t i = 2; i < parts.size(); ++i) {
		if (!current) return nullptr;
		const auto& contents = current->GetContents();
		auto it = contents.find(parts[i]);
		if (it == contents.end()) return nullptr;
		current = it->second;
	}
	return current;
}

Asset* Player::RemoveByPath(const string& path) {
	auto parts = SplitPath(path);
	if (parts.empty()) return nullptr;

	if (parts.size() <= 2) {
		Asset** slot = nullptr;
		if (parts[0] == "left") slot = &leftHand;
		else if (parts[0] == "right") slot = &rightHand;
		else if (parts[0] == "back") slot = &backPack;
		else if (parts[0] == "room") return nullptr;
		if (!slot) return nullptr;
		if (parts.size() == 2 && (!*slot || (*slot)->GetName() != parts[1])) return nullptr;
		Asset* removed = *slot;
		*slot = nullptr;
		return removed;
	}

	string parentPath;
	for (size_t i = 0; i < parts.size() - 1; ++i) {
		if (i > 0) parentPath += '/';
		parentPath += parts[i];
	}
	Asset* parent = GetByPath(parentPath);
	if (!parent) return nullptr;

	const string& lastName = parts.back();
	const auto& contents = parent->GetContents();
	auto it = contents.find(lastName);
	if (it == contents.end()) return nullptr;
	Asset* removed = it->second;
	parent->RemoveContent(lastName);
	return removed;
}

bool Player::AddByPath(const string& path, Asset* asset) {
	if (!asset) return false;
	auto parts = SplitPath(path);
	if (parts.empty()) return false;

	if (parts.size() == 1) {
		if (parts[0] == "left") {
			if (asset->GetMobility() != ASSET_OBJECT || leftHand) return false;
			leftHand = asset;
			return true;
		}
		if (parts[0] == "right") {
			if (asset->GetMobility() != ASSET_OBJECT || rightHand) return false;
			rightHand = asset;
			return true;
		}
		if (parts[0] == "back") {
			if (asset->GetMobility() != ASSET_OBJECT || !asset->GetBackpack() || backPack) return false;
			backPack = asset;
			return true;
		}
		if (parts[0] == "room") {
			return false;
		}
		return false;
	}

	Asset* container = GetByPath(path);
	if (!container) return false;
	return container->AddContent(asset->GetName(), asset);
}

