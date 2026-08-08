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

AActor* AAssetBase::GetInstance(FString room, FString name) {
	auto it = assetInstances.find(TCHAR_TO_UTF8(*room));
	if (it == assetInstances.end()) return nullptr;
	AActor** found = it->second.Find(name);
	return found ? *found : nullptr;
}

void AAssetBase::AddInstance(FString room, FString name, AActor* actor) {
	auto& instances = assetInstances[TCHAR_TO_UTF8(*room)];
	if (!instances.Contains(name)) {
		instances.Add(name, actor);
	}
}

void AAssetBase::RemoveInstance(FString room, FString name, AActor*& instance) {
	auto it = assetInstances.find(TCHAR_TO_UTF8(*room));
	if (it != assetInstances.end() && it->second.Contains(name)) {
		instance = it->second[name];
		it->second.Remove(name);
		if (it->second.Num() == 0) {
			assetInstances.erase(it);
		}
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Asset not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

void AAssetBase::RemoveInstances(FString room, TArray<AActor*>& instances) {
	auto it = assetInstances.find(TCHAR_TO_UTF8(*room));
	if (it == assetInstances.end()) return;

	it->second.GenerateValueArray(instances);
	assetInstances.erase(it);
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
		entry.usage = a->GetUsage();
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
		entry.usage = a->GetUsage();
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

FString AAssetBase::PickAsset(FString path, FString target, FString& room, FAsset& asset) {
	if (!global) return FString();
	auto player = global->GetPlayer();
	if (!player) return FString();

	string pathStr = TCHAR_TO_UTF8(*path);
	string targetStr = TCHAR_TO_UTF8(*target);
	auto pathParts = Player::SplitPath(pathStr);
	if (pathParts.empty() || pathParts[0] != "room" || pathParts.size() < 2) return FString();

	Room* currentRoom = GetCurrentRoom();
	if (!currentRoom) return FString();

	// 从房间移除资产
	Asset* pickedAsset = nullptr;
	if (pathParts.size() == 2) {
		pickedAsset = currentRoom->RemoveAsset(pathParts[1]);
	}
	else {
		Asset* parent = NavigateRoomAsset(currentRoom, pathParts, 1, pathParts.size() - 1);
		if (!parent) return FString();
		const string& lastName = pathParts.back();
		const auto& contents = parent->GetContents();
		auto it = contents.find(lastName);
		if (it == contents.end()) return FString();
		pickedAsset = it->second;
		parent->RemoveContent(lastName);
	}
	if (!pickedAsset) return FString();

	// 向部位或随身容器添加资产
	if (!player->AddByPath(targetStr, pickedAsset)) {
		if (pathParts.size() == 2) {
			currentRoom->AddAsset(pickedAsset);
		}
		else {
			Asset* parent = NavigateRoomAsset(currentRoom, pathParts, 1, pathParts.size() - 1);
			if (parent) parent->AddContent(pickedAsset->GetName(), pickedAsset);
			else currentRoom->AddAsset(pickedAsset);
		}
		return FString();
	}

	string newPath = targetStr + "/" + pickedAsset->GetName();
	room = UTF8_TO_TCHAR(currentRoom->GetNumber().data());
	asset.name = UTF8_TO_TCHAR(pickedAsset->GetName().data());
	asset.type = UTF8_TO_TCHAR(pickedAsset->GetType().data());
	asset.icon = UTF8_TO_TCHAR(pickedAsset->GetIcon().data());
	asset.mesh = UTF8_TO_TCHAR(pickedAsset->GetMesh().data());
	asset.isContainer = pickedAsset->GetVolume() > 0;
	asset.usage = pickedAsset->GetUsage();
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

bool AAssetBase::DropAsset(FString path, FString& room, FAsset& outAsset) {
	if (!global) return false;
	auto player = global->GetPlayer();
	if (!player) return false;

	string pathStr = TCHAR_TO_UTF8(*path);
	auto parts = Player::SplitPath(pathStr);
	if (parts.empty() || parts[0] == "room") return false;

	Asset* asset = player->RemoveByPath(pathStr);
	if (!asset) return false;

	Room* currentRoom = GetCurrentRoom();
	if (currentRoom) {
		currentRoom->AddAsset(asset);
		room = UTF8_TO_TCHAR(currentRoom->GetNumber().data());
		outAsset.name = UTF8_TO_TCHAR(asset->GetName().data());
		outAsset.type = UTF8_TO_TCHAR(asset->GetType().data());
		outAsset.icon = UTF8_TO_TCHAR(asset->GetIcon().data());
		outAsset.mesh = UTF8_TO_TCHAR(asset->GetMesh().data());
		outAsset.isContainer = asset->GetVolume() > 0;
		outAsset.usage = asset->GetUsage();
		outAsset.path = FString("room/") + UTF8_TO_TCHAR(asset->GetName().data());
		outAsset.location = FVector(asset->GetPositionX(), asset->GetPositionY(), asset->GetPositionZ());
		outAsset.scale = asset->GetScale();
	}
	else {
		delete asset;
	}
	return true;
}

bool AAssetBase::UseAsset(FString path) {
	if (!global) return false;
	auto player = global->GetPlayer();
	if (!player) return false;

	string pathStr = TCHAR_TO_UTF8(*path);
	auto parts = Player::SplitPath(pathStr);
	if (parts.empty()) return false;

	Asset* asset = nullptr;
	if (parts[0] == "room") {
		if (parts.size() < 2) return false;
		Room* room = GetCurrentRoom();
		if (!room) return false;
		asset = NavigateRoomAsset(room, parts, 1, parts.size());
	}
	else {
		asset = player->GetByPath(pathStr);
	}
	if (!asset) return false;

	int usage = asset->GetUsage();
	if (usage <= 0) return false;

	usage -= 1;
	asset->SetUsage(usage);
	return usage <= 0;
}

TArray<FAsset> AAssetBase::GetRoom() {
	TArray<FAsset> result;
	Room* room = GetCurrentRoom();
	if (!room) return result;

	for (auto a : room->GetAssets()) {
		FAsset entry;
		entry.name = UTF8_TO_TCHAR(a->GetName().data());
		entry.type = UTF8_TO_TCHAR(a->GetType().data());
		entry.icon = UTF8_TO_TCHAR(a->GetIcon().data());
		entry.mesh = UTF8_TO_TCHAR(a->GetMesh().data());
		entry.isContainer = a->GetVolume() > 0;
		entry.usage = a->GetUsage();
		entry.path = FString("room/") + UTF8_TO_TCHAR(a->GetName().data());
		entry.location = FVector(a->GetPositionX(), a->GetPositionY(), a->GetPositionZ());
		entry.scale = a->GetScale();
		result.Add(entry);
	}
	return result;
}

bool AAssetBase::SetPosition(FString asset, FVector position) {
	Room* room = GetCurrentRoom();
	if (!room) return false;

	string assetName = TCHAR_TO_UTF8(*asset);
	for (auto a : room->GetAssets()) {
		if (a->GetName() == assetName) {
			a->SetPosition(position.X, position.Y, position.Z);
			return true;
		}
	}
	return false;
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

