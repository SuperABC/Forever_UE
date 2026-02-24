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

	try {
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
		map->InitZones(modHandles);
		map->InitBuildings(modHandles);
		map->InitComponents(modHandles);
		map->InitRooms(modHandles);

		// 读取Populace相关类及Mod
		populace->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
		populace->ReadConfigs("configs/config_populace.json");
		populace->InitAssets(modHandles);
		populace->InitJobs(modHandles);
		populace->InitNames(modHandles);
		populace->InitSchedulers(modHandles);

		// 读取Society相关类及Mod
		society->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
		society->ReadConfigs("configs/config_society.json");
		society->InitCalendars(modHandles);
		society->InitOrganizations(modHandles);

		// 读取Story相关类及Mod
		story->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
		story->ReadConfigs("configs/config_story.json");
		story->InitEvents(modHandles);
		story->InitChanges(modHandles);

		// 读取Industry相关类及Mod
		industry->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
		industry->ReadConfigs("configs/config_industry.json");

		// 读取Traffic相关类及Mod
		traffic->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
		traffic->ReadConfigs("configs/config_traffic.json");

		// 读取Player相关类及Mod
		player->SetResourcePath(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/");
		player->ReadConfigs("configs/config_player.json");

		int size = 4;
		string path = "scripts/ys.json";

		story->Init();
		int accomodation = map->Init(size, size);
		populace->Init(accomodation, story->ReadNames("ys", path), player->GetTime());
		map->Checkin(populace->GetCitizens(), player->GetTime());
		society->Init(map, populace, player->GetTime());
		story->InitVariables(player->GetTime());
		story->ReadStory("ys", path);

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
		trafficActor = GetWorld()->SpawnActor<ATrafficBase>(TrafficClass, Location, Rotation);
		trafficActor->SetGlobal(this);
		populaceActor = GetWorld()->SpawnActor<APopulaceBase>(PopulaceClass, Location, Rotation);
		populaceActor->SetGlobal(this);
		storyActor = GetWorld()->SpawnActor<AStoryBase>(StoryClass, Location, Rotation);
		storyActor->SetGlobal(this);
	}
	catch (ExceptionBase& e) {
		UE_LOGFMT(LogTemp, Log, "{0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	}
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

ATerrainBase* AGlobalBase::GetTerrainActor() {
	return terrainActor;
}

ARoadnetBase* AGlobalBase::GetRoadnetActor() {
	return roadnetActor;
}

AZoneBase* AGlobalBase::GetZoneActor() {
	return zoneActor;
}

ABuildingBase* AGlobalBase::GetBuildingActor() {
	return buildingActor;
}

ARoomBase* AGlobalBase::GetRoomActor() {
	return roomActor;
}

ATrafficBase* AGlobalBase::GetTrafficActor() {
	return trafficActor;
}

APopulaceBase* AGlobalBase::GetPopulaceActor() {
	return populaceActor;
}

AStoryBase* AGlobalBase::GetStoryActor() {
	return storyActor;
}



