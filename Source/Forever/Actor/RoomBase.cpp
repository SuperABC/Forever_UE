#include "Actor/RoomBase.h"


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
		roomInfo.name = UTF8_TO_TCHAR(room->GetAddress().data());
		auto pos = room->GetPosition(room->GetSizeX() / 2.f, room->GetSizeY() / 2.f);
		roomInfo.center = FVector(pos.first, pos.second, (room->GetLayer() + 0.5f) * building->GetHeight());
		roomInfo.size = FVector(room->GetSizeX(), room->GetSizeY(), building->GetHeight());
		roomInfo.rotation = building->GetParentPlot()->GetRotation();
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

}

void ARoomBase::LeaveRoom(FString room) {

}


