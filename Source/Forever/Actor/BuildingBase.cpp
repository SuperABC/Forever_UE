#include "Actor/BuildingBase.h"
#include "Actor/GlobalBase.h"

#include "utility.h"
#include "error.h"


using namespace std;

ABuildingBase::ABuildingBase() {
	PrimaryActorTick.bCanEverTick = true;
}

ABuildingBase::~ABuildingBase() {

}

void ABuildingBase::BeginPlay() {
	Super::BeginPlay();
}

void ABuildingBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector location = FVector(0.f, 0.f, 0.f);
	((AGlobalBase*)global)->GetLocation(location);
	location /= 1000.f;

	TArray<FBuilding> buildings;
	auto plots = ((AGlobalBase*)global)->GetMap()->GetRoadnet()->GetPlots();
	for (auto plot : plots) {
		FVector plotLocation = FVector(plot->GetPosX(), plot->GetPosY(), 0.f);
		if ((location - plotLocation).Size() > 64.f) {
			continue;
		}
		auto plotBuildings = plot->GetBuildings();
		for (auto plotBuilding : plotBuildings) {
			if (plotBuilding.second->GetAcreage() <= 0.f)continue;
			if (buildingInstances.find(plotBuilding.first) != buildingInstances.end()) {
				continue;
			}
			auto building = plotBuilding.second;
			FBuilding buildingInfo;
			buildingInfo.name = UTF8_TO_TCHAR(plotBuilding.first.data());
			auto construction = building->GetConstruction();
			auto center = plot->GetPosition(
				building->GetPosX() - building->GetSizeX() / 2.f + construction.GetPosX(),
				building->GetPosY() - building->GetSizeY() / 2.f + construction.GetPosY());
			buildingInfo.center = FVector(center.first, center.second, 0.f);
			if ((location - buildingInfo.center).Size() > 32.f) {
				continue;
			}
			buildingInfo.size = FVector(construction.GetSizeX(), construction.GetSizeY(), 1.f);
			buildingInfo.rotation = plot->GetRotation();
			ConstructBuilding(building, buildingInfo);
			buildings.Add(buildingInfo);
		}
		auto plotZones = plot->GetZones();
		for (auto plotZone : plotZones) {
			auto zone = plotZone.second;
			for(auto zoneBuilding : zone->GetBuildings()) {
				if (zoneBuilding.second->GetAcreage() <= 0.f)continue;
				if (buildingInstances.find(zoneBuilding.first) != buildingInstances.end()) {
					continue;
				}
				auto building = zoneBuilding.second;
				FBuilding buildingInfo;
				buildingInfo.name = UTF8_TO_TCHAR(zoneBuilding.first.data());
				auto construction = building->GetConstruction();
				auto center = plot->GetPosition(
					zone->GetPosX() - zone->GetSizeX() / 2 + building->GetPosX() - building->GetSizeX() / 2.f + construction.GetPosX(),
					zone->GetPosY() - zone->GetSizeY() / 2 + building->GetPosY() - building->GetSizeY() / 2.f + construction.GetPosY());
				buildingInfo.center = FVector(center.first, center.second, 0.f);
				if ((location - buildingInfo.center).Size() > 32.f) {
					continue;
				}
				buildingInfo.size = FVector(construction.GetSizeX(), construction.GetSizeY(), 1.f);
				buildingInfo.rotation = plot->GetRotation();
				ConstructBuilding(building, buildingInfo);
				buildings.Add(buildingInfo);
			}
		}
	}
	UpdateBuilding(buildings);
}

void ABuildingBase::SetGlobal(AActor* g) {
	this->global = g;
}

void ABuildingBase::SetInstance(FString name, AActor* actor) {
	if (buildingInstances.find(TCHAR_TO_UTF8(*name)) == buildingInstances.end()) {
		buildingInstances[TCHAR_TO_UTF8(*name)] = actor;
	}
	else {
		THROW_EXCEPTION(InvalidConfigException, string("Duplicate building name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

void ABuildingBase::ConstructBuilding(Building* building, FBuilding& info) {
	auto construction = building->GetConstruction();
	for (int i = 0; i < building->GetLayers(); i++) {
		info.walls.Add(
			FWall(FVector(0.f, 0.f, 0.4f * (i + 1)),
				FVector(construction.GetSizeX(), construction.GetSizeY(), 0.01f)));
	}
	for (auto room : building->GetRooms()) {
		info.walls.Add(
			FWall(FVector(room->GetPosX() + room->GetSizeX() / 2.f, room->GetPosY(), 0.35f + 0.4f * room->GetLayer()) - FVector(construction.GetSizeX() / 2.f, construction.GetSizeY() / 2.f, 0.f),
				FVector(0.01f, room->GetSizeY(), 0.1f)));
		info.walls.Add(
			FWall(FVector(room->GetPosX() - room->GetSizeX() / 2.f, room->GetPosY(), 0.35f + 0.4f * room->GetLayer()) - FVector(construction.GetSizeX() / 2.f, construction.GetSizeY() / 2.f, 0.f),
				FVector(0.01f, room->GetSizeY(), 0.1f)));
		info.walls.Add(
			FWall(FVector(room->GetPosX(), room->GetPosY() + room->GetSizeY() / 2.f, 0.35f + 0.4f * room->GetLayer()) - FVector(construction.GetSizeX() / 2.f, construction.GetSizeY() / 2.f, 0.f),
				FVector(room->GetSizeX(), 0.01f, 0.1f)));
		info.walls.Add(
			FWall(FVector(room->GetPosX(), room->GetPosY() - room->GetSizeY() / 2.f, 0.35f + 0.4f * room->GetLayer()) - FVector(construction.GetSizeX() / 2.f, construction.GetSizeY() / 2.f, 0.f),
				FVector(room->GetSizeX(), 0.01f, 0.1f)));
	}
}
