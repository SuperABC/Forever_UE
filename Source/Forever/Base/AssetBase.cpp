#include "AssetBase.h"

#include "player/player.h"
#include "player/asset.h"
#include "map/room.h"
#include "map/map.h"
#include "story/story.h"


using namespace std;

static Asset* NavigateRoomAsset(Room* room, const vector<string>& parts, size_t from, size_t to) {
	if (from >= to || from >= parts.size()) return nullptr;
	Asset* cur = nullptr;
	for (auto a : room->GetAssets()) {
		if (a->GetName() == parts[from]) { cur = a; break; }
	}
	for (size_t i = from + 1; i < to && cur; ++i) {
		const auto& contents = cur->GetContents();
		auto it = contents.find(parts[i]);
		cur = (it != contents.end()) ? it->second : nullptr;
	}
	return cur;
}

AAssetBase::AAssetBase() {
	PrimaryActorTick.bCanEverTick = true;
}

AAssetBase::~AAssetBase() {

}

void AAssetBase::BeginPlay() {
	Super::BeginPlay();
}

void AAssetBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!global) return;
}

void AAssetBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

Room* AAssetBase::GetCurrentRoom() const {
	if (!global) return nullptr;
	auto story = global->GetStory();
	auto map = global->GetMap();
	if (!story || !map) return nullptr;
	return story->GetCurrentRoom(map);
}

AActor* AAssetBase::GetInstance(FString name) {
	auto it = assetInstances.find(TCHAR_TO_UTF8(*name));
	if (it == assetInstances.end()) return nullptr;
	return it->second;
}

void AAssetBase::AddInstance(FString name, AActor* actor) {
	if (assetInstances.find(TCHAR_TO_UTF8(*name)) == assetInstances.end()) {
		assetInstances[TCHAR_TO_UTF8(*name)] = actor;
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Duplicate asset name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

void AAssetBase::RemoveInstance(FString name, AActor*& instance) {
	if (assetInstances.find(TCHAR_TO_UTF8(*name)) != assetInstances.end()) {
		instance = assetInstances[TCHAR_TO_UTF8(*name)];
		assetInstances.erase(TCHAR_TO_UTF8(*name));
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Asset not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

FAsset AAssetBase::GetAsset(FString path) {
	if (!global) return FAsset();
	auto player = global->GetPlayer();
	if (!player) return FAsset();

	string pathStr = TCHAR_TO_UTF8(*path);
	auto parts = Player::SplitPath(pathStr);
	if (parts.empty()) return FAsset();

	auto makeEntry = [](Asset* a) {
		FAsset entry;
		entry.name = UTF8_TO_TCHAR(a->GetName().data());
		entry.type = UTF8_TO_TCHAR(a->GetType().data());
		entry.icon = UTF8_TO_TCHAR(a->GetIcon().data());
		entry.mesh = UTF8_TO_TCHAR(a->GetMesh().data());
		entry.isContainer = a->GetVolume() > 0;
		entry.usable = a->GetUsable();
		return entry;
	};

	if (parts[0] == "room") {
		if (parts.size() < 2) return FAsset();
		Room* room = GetCurrentRoom();
		if (!room) return FAsset();
		Asset* asset = NavigateRoomAsset(room, parts, 1, parts.size());
		if (!asset) return FAsset();
		return makeEntry(asset);
	}

	Asset* asset = player->GetByPath(pathStr);
	if (!asset) return FAsset();
	return makeEntry(asset);
}

TArray<FAsset> AAssetBase::GetAssets(FString path) {
	TArray<FAsset> result;
	if (!global) return result;
	auto player = global->GetPlayer();
	if (!player) return result;

	string pathStr = TCHAR_TO_UTF8(*path);
	auto parts = Player::SplitPath(pathStr);
	if (parts.empty()) return result;

	auto makeEntry = [](Asset* a) {
		FAsset entry;
		entry.name = UTF8_TO_TCHAR(a->GetName().data());
		entry.type = UTF8_TO_TCHAR(a->GetType().data());
		entry.icon = UTF8_TO_TCHAR(a->GetIcon().data());
		entry.mesh = UTF8_TO_TCHAR(a->GetMesh().data());
		entry.isContainer = a->GetVolume() > 0;
		entry.usable = a->GetUsable();
		return entry;
	};

	if (parts[0] == "room") {
		Room* room = GetCurrentRoom();
		if (!room) return result;
		if (parts.size() == 1) {
			for (auto a : room->GetAssets()) result.Add(makeEntry(a));
			return result;
		}
		Asset* container = NavigateRoomAsset(room, parts, 1, parts.size());
		if (!container) return result;
		for (const auto& pair : container->GetContents()) result.Add(makeEntry(pair.second));
		return result;
	}

	if (parts.size() == 1) {
		Asset* asset = player->GetByPath(pathStr);
		if (asset) result.Add(makeEntry(asset));
		return result;
	}

	Asset* container = player->GetByPath(pathStr);
	if (!container) return result;
	for (const auto& pair : container->GetContents()) result.Add(makeEntry(pair.second));
	return result;
}

FString AAssetBase::PickAsset(FString path, FString target, FAsset& outAsset) {
	if (!global) return FString();
	auto player = global->GetPlayer();
	if (!player) return FString();

	string pathStr = TCHAR_TO_UTF8(*path);
	string targetStr = TCHAR_TO_UTF8(*target);
	auto pathParts = Player::SplitPath(pathStr);
	if (pathParts.empty() || pathParts[0] != "room" || pathParts.size() < 2) return FString();

	Room* room = GetCurrentRoom();
	if (!room) return FString();

	// 从房间移除资产
	Asset* asset = nullptr;
	if (pathParts.size() == 2) {
		asset = room->RemoveAsset(pathParts[1]);
	}
	else {
		Asset* parent = NavigateRoomAsset(room, pathParts, 1, pathParts.size() - 1);
		if (!parent) return FString();
		const string& lastName = pathParts.back();
		const auto& contents = parent->GetContents();
		auto it = contents.find(lastName);
		if (it == contents.end()) return FString();
		asset = it->second;
		parent->RemoveContent(lastName);
	}
	if (!asset) return FString();

	// 向部位或随身容器添加资产
	if (!player->AddByPath(targetStr, asset)) {
		if (pathParts.size() == 2) {
			room->AddAsset(asset);
		}
		else {
			Asset* parent = NavigateRoomAsset(room, pathParts, 1, pathParts.size() - 1);
			if (parent) parent->AddContent(asset->GetName(), asset);
			else room->AddAsset(asset);
		}
		return FString();
	}

	string newPath = targetStr + "/" + asset->GetName();
	outAsset.name = UTF8_TO_TCHAR(asset->GetName().data());
	outAsset.type = UTF8_TO_TCHAR(asset->GetType().data());
	outAsset.icon = UTF8_TO_TCHAR(asset->GetIcon().data());
	outAsset.mesh = UTF8_TO_TCHAR(asset->GetMesh().data());
	outAsset.isContainer = asset->GetVolume() > 0;
	outAsset.usable = asset->GetUsable();
	return UTF8_TO_TCHAR(newPath.data());
}

bool AAssetBase::DestroyAsset(FString path) {
	if (!global) return false;
	auto player = global->GetPlayer();
	if (!player) return false;

	string pathStr = TCHAR_TO_UTF8(*path);
	auto parts = Player::SplitPath(pathStr);
	if (parts.empty()) return false;

	if (parts[0] == "room") {
		if (parts.size() < 2) return false;
		Room* room = GetCurrentRoom();
		if (!room) return false;
		Asset* asset = nullptr;
		if (parts.size() == 2) {
			asset = room->RemoveAsset(parts[1]);
		}
		else {
			Asset* parent = NavigateRoomAsset(room, parts, 1, parts.size() - 1);
			if (!parent) return false;
			const string& lastName = parts.back();
			const auto& contents = parent->GetContents();
			auto it = contents.find(lastName);
			if (it == contents.end()) return false;
			asset = it->second;
			parent->RemoveContent(lastName);
		}
		if (!asset) return false;
		delete asset;
		return true;
	}

	Asset* asset = player->RemoveByPath(pathStr);
	if (!asset) return false;
	delete asset;
	return true;
}

bool AAssetBase::DropAsset(FString path) {
	if (!global) return false;
	auto player = global->GetPlayer();
	if (!player) return false;

	string pathStr = TCHAR_TO_UTF8(*path);
	auto parts = Player::SplitPath(pathStr);
	if (parts.empty() || parts[0] == "room") return false;

	Asset* asset = player->RemoveByPath(pathStr);
	if (!asset) return false;

	Room* room = GetCurrentRoom();
	if (room) room->AddAsset(asset);
	else delete asset;
	return true;
}

bool AAssetBase::DestroyAssets(FString path) {
	if (!global) return false;
	auto player = global->GetPlayer();
	if (!player) return false;

	string pathStr = TCHAR_TO_UTF8(*path);
	auto parts = Player::SplitPath(pathStr);
	if (parts.empty()) return false;

	if (parts[0] == "room") {
		Room* room = GetCurrentRoom();
		if (!room) return false;
		if (parts.size() == 1) {
			room->ClearAssets();
			return true;
		}
		Asset* container = NavigateRoomAsset(room, parts, 1, parts.size());
		if (!container) return false;
		container->RemoveContents();
		return true;
	}

	if (parts.size() == 1) {
		Asset* asset = player->RemoveByPath(pathStr);
		if (!asset) return false;
		delete asset;
		return true;
	}

	Asset* container = player->GetByPath(pathStr);
	if (!container) return false;
	container->RemoveContents();
	return true;
}

