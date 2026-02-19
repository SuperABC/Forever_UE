#include "Actor/ZoneBase.h"
#include "Actor/GlobalBase.h"

#include "utility.h"
#include "error.h"


using namespace std;

AZoneBase::AZoneBase() {
	PrimaryActorTick.bCanEverTick = true;
}

AZoneBase::~AZoneBase() {

}

void AZoneBase::BeginPlay() {
	Super::BeginPlay();
}

void AZoneBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector location = FVector(0.f, 0.f, 0.f);
	((AGlobalBase*)global)->GetLocation(location);
	location /= 1000.f;

	TArray<FZone> zones;
	auto plots = ((AGlobalBase*)global)->GetMap()->GetRoadnet()->GetPlots();
	for(auto plot : plots) {
		FVector plotLocation = FVector(plot->GetPosX(), plot->GetPosY(), 0.f);
		if((location - plotLocation).Size() > 64.f) {
			continue;
		}
		auto plotZones = plot->GetZones();
		for (auto plotZone : plotZones) {
			if(zoneInstances.find(plotZone.first) != zoneInstances.end()) {
				continue;
			}
			auto zone = plotZone.second;
			FZone zoneInfo;
			zoneInfo.name = UTF8_TO_TCHAR(plotZone.first.data());
			auto center = plot->GetPosition(zone->GetPosX(), zone->GetPosY());
			zoneInfo.center = FVector(center.first, center.second, 0.f);
			zoneInfo.size = FVector(zone->GetSizeX(), zone->GetSizeY(), 1.f);
			zoneInfo.rotation = plot->GetRotation();
			zones.Add(zoneInfo);
		}
	}
	UpdateZone(zones);
}

void AZoneBase::SetGlobal(AActor* g) {
	this->global = g;
}

void AZoneBase::SetInstance(FString name, AActor* actor) {
	if (zoneInstances.find(TCHAR_TO_UTF8(*name)) == zoneInstances.end()) {
		zoneInstances[TCHAR_TO_UTF8(*name)] = actor;
	}
	else {
		THROW_EXCEPTION(InvalidConfigException, string("Duplicate zone name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}
