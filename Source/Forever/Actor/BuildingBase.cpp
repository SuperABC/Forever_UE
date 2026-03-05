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
	FVector bias = -FVector(construction.GetSizeX() / 2.f, construction.GetSizeY() / 2.f, 0.f);
	for (int i = 0; i < building->GetLayers(); i++) {
		auto ceilings = building->GetFloor(i)->GetCeilings();
		for (auto ceiling : ceilings) {
			info.walls.Add(
				FWall(FVector(ceiling.GetPosX(), ceiling.GetPosY(), 0.4f * (i + 1)) + bias,
					FVector(ceiling.GetSizeX(), ceiling.GetSizeY(), 0.01f)));
		}
	}
	for (auto room : building->GetRooms()) {
		info.walls.Append(ConstructQuad(
			FVector(room->GetPosX(), room->GetPosY(), (room->GetLayer() + 0.5f) * building->GetHeight()) + bias,
			FVector(room->GetSizeX(), room->GetSizeY(), building->GetHeight()), { true, true, true, true },
			room->GetDoors(), room->GetWindows()));
	}
}

TArray<FWall> ABuildingBase::ConstructQuad(FVector center, FVector size, vector<bool> directions,
	unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> doors,
	unordered_map<FACE_DIRECTION, vector<pair<vector<float>, Quad>>> windows) {
	TArray<FWall> walls;
	auto doorsWest = doors[FACE_WEST];
	auto windowsWest = windows[FACE_WEST];
	if (doorsWest.size() > 0) {
		auto door = doorsWest[0];
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f, center.Y - size.Y / 2.f + (door.first[0] * size.Y + door.first[1]) / 2.f, center.Z),
				FVector(0.01f, door.first[0] * size.Y + door.first[1], size.Z)));
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f, center.Y - size.Y / 2.f + ((door.first[4] + 1) * size.Y + door.first[5]) / 2.f, center.Z),
				FVector(0.01f, (1 - door.first[4]) * size.Y - door.first[5], size.Z)));
	}
	else if (windowsWest.size() > 0) {
		auto window = windowsWest[0];
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f, center.Y - size.Y / 2.f + (window.first[0] * size.Y + window.first[1]) / 2.f, center.Z),
				FVector(0.01f, window.first[0] * size.Y + window.first[1], size.Z)));
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f, center.Y - size.Y / 2.f + ((window.first[4] + 1) * size.Y + window.first[5]) / 2.f, center.Z),
				FVector(0.01f, (1 - window.first[4]) * size.Y - window.first[5], size.Z)));
	}
	else {
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f, center.Y, center.Z),
				FVector(0.01f, size.Y, size.Z)));
	}
	auto doorsEast = doors[FACE_EAST];
	auto windowsEast = windows[FACE_EAST];
	if (doorsEast.size() > 0) {
		auto door = doorsEast[0];
		walls.Add(
			FWall(FVector(center.X + size.X / 2.f, center.Y - size.Y / 2.f + (door.first[0] * size.Y + door.first[1]) / 2.f, center.Z),
				FVector(0.01f, door.first[0] * size.Y + door.first[1], size.Z)));
		walls.Add(
			FWall(FVector(center.X + size.X / 2.f, center.Y - size.Y / 2.f + ((door.first[4] + 1) * size.Y + door.first[5]) / 2.f, center.Z),
				FVector(0.01f, (1 - door.first[4]) * size.Y - door.first[5], size.Z)));
	}
	else if (windowsEast.size() > 0) {
		auto window = windowsEast[0];
		walls.Add(
			FWall(FVector(center.X + size.X / 2.f, center.Y - size.Y / 2.f + (window.first[0] * size.Y + window.first[1]) / 2.f, center.Z),
				FVector(0.01f, window.first[0] * size.Y + window.first[1], size.Z)));
		walls.Add(
			FWall(FVector(center.X + size.X / 2.f, center.Y - size.Y / 2.f + ((window.first[4] + 1) * size.Y + window.first[5]) / 2.f, center.Z),
				FVector(0.01f, (1 - window.first[4]) * size.Y - window.first[5], size.Z)));
	}
	else {
		walls.Add(
			FWall(FVector(center.X + size.X / 2.f, center.Y, center.Z),
				FVector(0.01f, size.Y, size.Z)));
	}
	auto doorsNorth = doors[FACE_NORTH];
	auto windowsNorth = windows[FACE_NORTH];
	if (doorsNorth.size() > 0) {
		auto door = doorsNorth[0];
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f + (door.first[0] * size.X + door.first[1]) / 2.f, center.Y - size.Y / 2.f, center.Z),
				FVector(door.first[0] * size.X + door.first[1], 0.01f, size.Z)));
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f + ((door.first[4] + 1) * size.X + door.first[5]) / 2.f, center.Y - size.Y / 2.f, center.Z),
				FVector((1 - door.first[4]) * size.X - door.first[5], 0.01f, size.Z)));
	}
	else if (windowsNorth.size() > 0) {
		auto window = windowsNorth[0];
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f + (window.first[0] * size.X + window.first[1]) / 2.f, center.Y - size.Y / 2.f, center.Z),
				FVector(window.first[0] * size.X + window.first[1], 0.01f, size.Z)));
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f + ((window.first[4] + 1) * size.X + window.first[5]) / 2.f, center.Y - size.Y / 2.f, center.Z),
				FVector((1 - window.first[4]) * size.X - window.first[5], 0.01f, size.Z)));
	}
	else {
		walls.Add(
			FWall(FVector(center.X, center.Y - size.Y / 2.f, center.Z),
				FVector(size.X, 0.01f, size.Z)));
	}
	auto doorsSouth = doors[FACE_SOUTH];
	auto windowsSouth = windows[FACE_SOUTH];
	if (doorsSouth.size() > 0) {
		auto door = doorsSouth[0];
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f + (door.first[0] * size.X + door.first[1]) / 2.f, center.Y + size.Y / 2.f, center.Z),
				FVector(door.first[0] * size.X + door.first[1], 0.01f, size.Z)));
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f + ((door.first[4] + 1) * size.X + door.first[5]) / 2.f, center.Y + size.Y / 2.f, center.Z),
				FVector((1 - door.first[4]) * size.X - door.first[5], 0.01f, size.Z)));
	}
	else if (windowsSouth.size() > 0) {
		auto window = windowsSouth[0];
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f + (window.first[0] * size.X + window.first[1]) / 2.f, center.Y + size.Y / 2.f, center.Z),
				FVector(window.first[0] * size.X + window.first[1], 0.01f, size.Z)));
		walls.Add(
			FWall(FVector(center.X - size.X / 2.f + ((window.first[4] + 1) * size.X + window.first[5]) / 2.f, center.Y + size.Y / 2.f, center.Z),
				FVector((1 - window.first[4]) * size.X - window.first[5], 0.01f, size.Z)));
	}
	else {
		walls.Add(
			FWall(FVector(center.X, center.Y + size.Y / 2.f, center.Z),
				FVector(size.X, 0.01f, size.Z)));
	}
	return walls;
}
