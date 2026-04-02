#include "Actor/RoomBase.h"
#include "Actor/GlobalBase.h"

#include "map/building.h"
#include "story/script.h"


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

void ARoomBase::SetGlobal(AActor* g) {
	this->global = g;
}

void ARoomBase::AddBuilding(string name, Building* building) {
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

void ARoomBase::RemoveBuilding(string name) {
	UpdateRoom(UTF8_TO_TCHAR(name.data()), {}, roomInstances[name]);
}

void ARoomBase::SetInstance(FString name, AActor* room) {
	roomInstances[TCHAR_TO_UTF8(*name)].Add(room);
}

void ARoomBase::RemoveInstance(FString name) {
	roomInstances.erase(TCHAR_TO_UTF8(*name));
}

void ARoomBase::EnterRoom(FString room) {
	auto storyBase = ((AGlobalBase*)global)->GetStoryActor();
	auto story = ((AGlobalBase*)global)->GetStory();
	auto zone = ((AGlobalBase*)global)->GetStory()->GetScript()->GetValue("player.zone").second;
	auto building = ((AGlobalBase*)global)->GetStory()->GetScript()->GetValue("player.building").second;
	Event* event;
	if (holds_alternative<string>(zone)) {
		if (holds_alternative<string>(building)) {
			event = new EnterRoomEvent(
				TCHAR_TO_UTF8(*get<string>(zone).data()),
				TCHAR_TO_UTF8(*get<string>(building).data()),
				TCHAR_TO_UTF8(*room));
		}
		else {
			event = new EnterRoomEvent(
				TCHAR_TO_UTF8(*get<string>(zone).data()), "", TCHAR_TO_UTF8(*room));
		}
	}
	else {
		if (holds_alternative<string>(building)) {
			event = new EnterRoomEvent(
				"", TCHAR_TO_UTF8(*get<string>(building).data()), TCHAR_TO_UTF8(*room));
		}
		else {
			event = new EnterRoomEvent(
				"", "", TCHAR_TO_UTF8(*room));
		}
	}

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	storyBase->MatchEvent(event, story->GetScript(), getValues);

	delete event;
}

void ARoomBase::LeaveRoom(FString room) {
	auto storyBase = ((AGlobalBase*)global)->GetStoryActor();
	auto story = ((AGlobalBase*)global)->GetStory();
	auto zone = ((AGlobalBase*)global)->GetStory()->GetScript()->GetValue("player.zone").second;
	auto building = ((AGlobalBase*)global)->GetStory()->GetScript()->GetValue("player.building").second;
	Event* event;
	if (holds_alternative<string>(zone)) {
		if (holds_alternative<string>(building)) {
			event = new LeaveRoomEvent(
				TCHAR_TO_UTF8(*get<string>(zone).data()),
				TCHAR_TO_UTF8(*get<string>(building).data()),
				TCHAR_TO_UTF8(*room));
		}
		else {
			event = new LeaveRoomEvent(
				TCHAR_TO_UTF8(*get<string>(zone).data()), "", TCHAR_TO_UTF8(*room));
		}
	}
	else {
		if (holds_alternative<string>(building)) {
			event = new LeaveRoomEvent(
				"", TCHAR_TO_UTF8(*get<string>(building).data()), TCHAR_TO_UTF8(*room));
		}
		else {
			event = new LeaveRoomEvent(
				"", "", TCHAR_TO_UTF8(*room));
		}
	}

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	storyBase->MatchEvent(event, story->GetScript(), getValues);

	delete event;
}


