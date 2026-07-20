#include "TrafficBase.h"

#include "GlobalBase.h"

#include "map/building.h"
#include "map/room.h"
#include "traffic/traffic.h"
#include "traffic/vehicle.h"


using namespace std;

ATrafficBase::ATrafficBase() {
	PrimaryActorTick.bCanEverTick = true;
}

ATrafficBase::~ATrafficBase() {

}

void ATrafficBase::BeginPlay() {
	Super::BeginPlay();
}

void ATrafficBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	static int step = 0;
	static int stride = 20;

	if (!global) return;

	FVector location = FVector(0.f, 0.f, 0.f);
	global->GetLocation(location);
	location /= 1000.f;

	TArray<FVehicle> adds;
	auto traffic = global->GetTraffic();
	if (!traffic) return;
	for (int j = step; j < traffic->GetVehicles().size(); j += stride) {
		auto vehicle = traffic->GetVehicles()[j];
		if (vehicleInstances.Contains(UTF8_TO_TCHAR(vehicle->GetName().c_str()))) {
			continue;
		}
		FVehicle vehicleInfo;
		vehicleInfo.name = UTF8_TO_TCHAR(vehicle->GetName().data());
		vehicleInfo.path = UTF8_TO_TCHAR(vehicle->GetPath().data());
		auto room = vehicle->GetRoom();
		auto xyzr = vehicle->GetPosition();
		if (!room) {
			vehicleInfo.pos = FVector(xyzr[0], xyzr[1], xyzr[2]);
		}
		else {
			auto pos = room->GetPosition(xyzr[0], xyzr[1]);
			vehicleInfo.pos = FVector(pos.first, pos.second, room->GetLayer() * room->GetParentBuilding()->GetHeight());
		}
		if ((location - vehicleInfo.pos).Size() > 4.f) {
			continue;
		}
		vehicleInfo.rotation = xyzr[3];
		adds.Add(vehicleInfo);
	}

	TArray<FString> removes;
	for (auto& [name, instance] : vehicleInstances) {
		if ((instance->GetActorLocation() / 1000.f - location).Size() > 8.f) {
			removes.Add(name);
		}
	}
	UpdateTraffic(adds, removes);

	step = (step + 1) % stride;

}

void ATrafficBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

AActor* ATrafficBase::GetInstance(FString name) {
	AActor** found = vehicleInstances.Find(name);
	return found ? *found : nullptr;
}

void ATrafficBase::AddInstance(FString name, AActor* actor) {
	if (!vehicleInstances.Contains(name)) {
		vehicleInstances.Add(name, actor);
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Duplicate vehicle name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

void ATrafficBase::RemoveInstance(FString name, AActor*& instance) {
	if (vehicleInstances.Contains(name)) {
		instance = vehicleInstances[name];
		vehicleInstances.Remove(name);
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Vehicle not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

