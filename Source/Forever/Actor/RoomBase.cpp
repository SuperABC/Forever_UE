#include "Actor/RoomBase.h"
#include "Actor/GlobalBase.h"


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

void ARoomBase::AddBuilding(std::string name, Building* building) {
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

void ARoomBase::RemoveBuilding(std::string name) {
	UpdateRoom(UTF8_TO_TCHAR(name.data()), {}, roomInstances[name]);
}

void ARoomBase::SetInstance(FString name, AActor* room) {
	roomInstances[TCHAR_TO_UTF8(*name)].Add(room);
}

void ARoomBase::RemoveInstance(FString name) {
	roomInstances.erase(TCHAR_TO_UTF8(*name));
}

void ARoomBase::EnterRoom(FString room) {
	//auto story = ((AGlobalBase*)global)->GetStoryActor();
	//auto zone = ((AGlobalBase*)global)->GetStory()->GetValue("player.zone").second;
	//auto building = ((AGlobalBase*)global)->GetStory()->GetValue("player.building").second;
	//if (holds_alternative<string>(zone)) {
	//	if (holds_alternative<string>(building)) {
	//		story->EnterRoom(UTF8_TO_TCHAR(get<std::string>(zone).data()),
	//			UTF8_TO_TCHAR(get<std::string>(building).data()), room);
	//	}
	//	else {
	//		story->EnterRoom(UTF8_TO_TCHAR(get<std::string>(zone).data()), "", room);
	//	}
	//}
	//else {
	//	if (holds_alternative<string>(building)) {
	//		story->EnterRoom("", UTF8_TO_TCHAR(get<std::string>(building).data()), room);
	//	}
	//	else {
	//		story->EnterRoom("", "", room);
	//	}
	//}

	//((AGlobalBase*)global)->GetStory()->SetValue("player.room", TCHAR_TO_UTF8(*room));
}

void ARoomBase::LeaveRoom(FString room) {
	//((AGlobalBase*)global)->GetStory()->SetValue("player.room", "");

	//auto story = ((AGlobalBase*)global)->GetStoryActor();
	//auto zone = ((AGlobalBase*)global)->GetStory()->GetValue("player.zone").second;
	//auto building = ((AGlobalBase*)global)->GetStory()->GetValue("player.building").second;
	//if (holds_alternative<string>(zone)) {
	//	if (holds_alternative<string>(building)) {
	//		story->LeaveRoom(UTF8_TO_TCHAR(get<std::string>(zone).data()),
	//			UTF8_TO_TCHAR(get<std::string>(building).data()), room);
	//	}
	//	else {
	//		story->LeaveRoom(UTF8_TO_TCHAR(get<std::string>(zone).data()), "", room);
	//	}
	//}
	//else {
	//	if (holds_alternative<string>(building)) {
	//		story->LeaveRoom("", UTF8_TO_TCHAR(get<std::string>(building).data()), room);
	//	}
	//	else {
	//		story->LeaveRoom("", "", room);
	//	}
	//}
}


