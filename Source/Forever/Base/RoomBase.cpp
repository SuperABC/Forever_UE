#include "RoomBase.h"

#include "GlobalBase.h"
#include "StoryBase.h"

#include "map/block.h"
#include "map/building.h"
#include "map/room.h"
#include "story/story.h"
#include "story/script.h"
#include "story/event.h"


using namespace std;

ARoomBase::ARoomBase() {
	PrimaryActorTick.bCanEverTick = true;
}

ARoomBase::~ARoomBase() {

}

void ARoomBase::BeginPlay() {
	Super::BeginPlay();
}

void ARoomBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ARoomBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

void ARoomBase::AddBuilding(const string& name, Building* building) {
	auto construction = building->GetConstruction();
	TArray<FRoom> rooms;
	for (auto room : building->GetRooms()) {
		FRoom roomInfo;
		roomInfo.name = UTF8_TO_TCHAR(room->GetNumber().data());
		auto pos = room->GetPosition(room->GetSizeX() / 2.f, room->GetSizeY() / 2.f);
		roomInfo.center = FVector(pos.first, pos.second, (room->GetLayer() + 0.5f) * building->GetHeight());
		roomInfo.size = FVector(room->GetSizeX(), room->GetSizeY(), building->GetHeight());
		roomInfo.rotation = building->GetParentBlock()->GetRotation();
		rooms.Add(roomInfo);
	}
	UpdateRoom(UTF8_TO_TCHAR(name.data()), rooms, {});
}

void ARoomBase::RemoveBuilding(const string& name) {
	UpdateRoom(UTF8_TO_TCHAR(name.data()), {}, roomInstances[name]);
}

void ARoomBase::SetInstance(FString name, AActor* room) {
	roomInstances[TCHAR_TO_UTF8(*name)].Add(room);
}

void ARoomBase::RemoveInstance(FString name) {
	roomInstances.erase(TCHAR_TO_UTF8(*name));
}

void ARoomBase::EnterRoom(FString room) {
	if (!global) return;
	auto storyBase = global->GetStoryActor();
	auto story = global->GetStory();
	if (!story) return;
	auto storyScript = story->GetScript();
	if (!storyScript) return;
	storyScript->SetValue("system.player.room", TCHAR_TO_UTF8(*room));
	auto zone = storyScript->GetValue("player.zone").second;
	auto building = storyScript->GetValue("player.building").second;
	Event* event;
	if (holds_alternative<string>(zone)) {
		if (holds_alternative<string>(building)) {
			event = new EnterRoomEvent(
				get<string>(zone), get<string>(building), TCHAR_TO_UTF8(*room));
		}
		else {
			event = new EnterRoomEvent(
				get<string>(zone), "", TCHAR_TO_UTF8(*room));
		}
	}
	else {
		if (holds_alternative<string>(building)) {
			event = new EnterRoomEvent(
				"", get<string>(building), TCHAR_TO_UTF8(*room));
		}
		else {
			event = new EnterRoomEvent(
				"", "", TCHAR_TO_UTF8(*room));
		}
	}

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return storyScript->GetValue(name);
		}
	};
	storyBase->MatchEvent(event, storyScript, getValues);

	delete event;
}

void ARoomBase::LeaveRoom(FString room) {
	if (!global) return;
	auto storyBase = global->GetStoryActor();
	auto story = global->GetStory();
	if (!story) return;
	auto storyScript = story->GetScript();
	if (!storyScript) return;
	storyScript->SetValue("system.player.room", "");
	auto zone = storyScript->GetValue("player.zone").second;
	auto building = storyScript->GetValue("player.building").second;
	Event* event;
	if (holds_alternative<string>(zone)) {
		if (holds_alternative<string>(building)) {
			event = new LeaveRoomEvent(
				get<string>(zone), get<string>(building), TCHAR_TO_UTF8(*room));
		}
		else {
			event = new LeaveRoomEvent(
				get<string>(zone), "", TCHAR_TO_UTF8(*room));
		}
	}
	else {
		if (holds_alternative<string>(building)) {
			event = new LeaveRoomEvent(
				"", get<string>(building), TCHAR_TO_UTF8(*room));
		}
		else {
			event = new LeaveRoomEvent(
				"", "", TCHAR_TO_UTF8(*room));
		}
	}

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](const string& name) -> pair<bool, ValueType> {
			return storyScript->GetValue(name);
		}
	};
	storyBase->MatchEvent(event, storyScript, getValues);

	delete event;
}


