#include "GlobalBase.h"

#include "utility.h"


using namespace std;

unordered_map<string, HMODULE> AGlobalBase::modHandles = {};

AGlobalBase::AGlobalBase() {
	PrimaryActorTick.bCanEverTick = true;
}

AGlobalBase::~AGlobalBase() {
	if (map) {
		delete map;
		map = nullptr;
	}
	if (populace) {
		delete populace;
		populace = nullptr;
	}
	if (society) {
		delete society;
		society = nullptr;
	}
	if (story) {
		delete story;
		story = nullptr;
	}
	if (industry) {
		delete industry;
		industry = nullptr;
	}
	if (traffic) {
		delete traffic;
		traffic = nullptr;
	}
	if (player) {
		delete player;
		player = nullptr;
	}
}

void AGlobalBase::BeginPlay() {
	Super::BeginPlay();

	map = new Map();
	populace = new Populace();
	society = new Society();
	story = new Story();
	industry = new Industry();
	traffic = new Traffic();
	player = new Player();

	// 读取Map相关类及Mod
	map->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
	map->ReadConfigs("configs/config_map.json");
	map->InitTerrains(modHandles);
	map->InitRoadnets(modHandles);

	// 读取Populace相关类及Mod
	populace->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
	populace->ReadConfigs("configs/config_populace.json");

	// 读取Society相关类及Mod
	society->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
	society->ReadConfigs("configs/config_society.json");

	// 读取Story相关类及Mod
	story->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
	story->ReadConfigs("configs/config_story.json");

	// 读取Industry相关类及Mod
	industry->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
	industry->ReadConfigs("configs/config_industry.json");

	// 读取Traffic相关类及Mod
	traffic->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
	traffic->ReadConfigs("configs/config_traffic.json");

	// 读取Player相关类及Mod
	player->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
	player->ReadConfigs("configs/config_player.json");

	int size = 4; map->Init(size, size);

	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	terrainActor = GetWorld()->SpawnActor<ATerrainBase>(TerrainClass, Location, Rotation);
	terrainActor->SetGlobal(this);
	terrainActor->InitInstances(size * BLOCK_SIZE, size * BLOCK_SIZE);
	roadnetActor = GetWorld()->SpawnActor<ARoadnetBase>(RoadnetClass, Location, Rotation);
	roadnetActor->SetGlobal(this);
	zoneActor = GetWorld()->SpawnActor<AZoneBase>(ZoneClass, Location, Rotation);
	zoneActor->SetGlobal(this);
	buildingActor = GetWorld()->SpawnActor<ABuildingBase>(BuildingClass, Location, Rotation);
	buildingActor->SetGlobal(this);
	roomActor = GetWorld()->SpawnActor<ARoomBase>(RoomClass, Location, Rotation);
	roomActor->SetGlobal(this);
	populaceActor = GetWorld()->SpawnActor<APopulaceBase>(PopulaceClass, Location, Rotation);
	populaceActor->SetGlobal(this);
	trafficActor = GetWorld()->SpawnActor<ATrafficBase>(TrafficClass, Location, Rotation);
	trafficActor->SetGlobal(this);
}

void AGlobalBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

Map* AGlobalBase::GetMap() {
	return map;
}

Populace* AGlobalBase::GetPopulace() {
	return populace;
}

Society* AGlobalBase::GetSociety() {
	return society;
}

Story* AGlobalBase::GetStory() {
	return story;
}

Industry* AGlobalBase::GetIndustry() {
	return industry;
}

Traffic *AGlobalBase::GetTraffic() {
	return traffic;
}

Player* AGlobalBase::GetPlayer() {
	return player;
}

