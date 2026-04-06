#include "GlobalBase.h"


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

		auto mods = Config::GetMods();

		// 读取Map相关类及Mod
		map->LoadConfigs();
		map->InitTerrains(modHandles, mods);
		map->InitRoadnets(modHandles, mods);
		map->InitZones(modHandles, mods);
		map->InitBuildings(modHandles, mods);
		map->InitComponents(modHandles, mods);
		map->InitRooms(modHandles, mods);

		// 读取Populace相关类及Mod
		populace->LoadConfigs();
		populace->InitAssets(modHandles, mods);
		populace->InitNames(modHandles, mods);
		populace->InitSchedulers(modHandles, mods);

		// 读取Society相关类及Mod
		society->LoadConfigs();
		society->InitCalendars(modHandles, mods);
		society->InitJobs(modHandles, mods);
		society->InitOrganizations(modHandles, mods);

		// 读取Story相关类及Mod
		story->LoadConfigs();
		story->InitScripts(modHandles, mods);

		// 读取Industry相关类及Mod
		industry->LoadConfigs();
		industry->InitProducts(modHandles, mods);
		industry->InitStorages(modHandles, mods);
		industry->InitManufactures(modHandles, mods);

		// 读取Traffic相关类及Mod
		traffic->LoadConfigs();
		traffic->InitRoutes(modHandles, mods);
		traffic->InitStations(modHandles, mods);
		traffic->InitVehicles(modHandles, mods);

		// 读取Player相关类及Mod
		player->LoadConfigs();
		player->InitSkills(modHandles, mods);

		int size = 4;

		player->Init();
		int accomodation = map->Init(size, size);
		populace->Init(accomodation, player);
		map->Checkin(populace, player);
		society->Init(map, populace, player);
		story->Init(map, populace, player);
		industry->Init(map);
		traffic->Init(map);

		FVector Location(0.0f, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		terrainActor = GetWorld()->SpawnActor<ATerrainBase>(TerrainClass, Location, Rotation);
		terrainActor->SetGlobal(this);
		terrainActor->InitInstances(size * CHUNK_SIZE, size * CHUNK_SIZE);
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

		auto pos = map->GetPlayerPos();
		SetLocation(FVector(pos.first, pos.second, 0.f));
	}
	catch (ExceptionBase& e) {
		debugf(e.GetDetailedInfo().data());
		UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	}
}

void AGlobalBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	player->Tick(DeltaTime);
	map->Tick(player);
	populace->Tick(player);
	society->Tick(player);
	story->Tick(player);
	industry->Tick(player);
	traffic->Tick(player);
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

void AGlobalBase::GlobalPause() {
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);
}

void AGlobalBase::GlobalResume() {
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

FStatus AGlobalBase::GetStatus() {
	auto time = player->GetTime();
	string timeStr = to_string(time->GetYear()) + "-" + to_string(time->GetMonth()) + "-" + to_string(time->GetDay()) + " " +
		to_string(time->GetHour()) + ":" + to_string(time->GetMinute()) + ":" + to_string(time->GetSecond());
	float chronode = time->GetHour() * 3600.f + time->GetMinute() * 60.f + time->GetSecond() + time->GetMillisecond() / 1000.f;
	chronode /= 24.f * 3600.f;
	return FStatus(FString(UTF8_TO_TCHAR(timeStr.data())), chronode);
}