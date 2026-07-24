#include "GlobalBase.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "Misc/CoreDelegates.h"
#include "Misc/PackageName.h"
#include "ShaderCodeLibrary.h"

#include "TerrainBase.h"
#include "RoadnetBase.h"
#include "ZoneBase.h"
#include "BuildingBase.h"
#include "RoomBase.h"
#include "PopulaceBase.h"
#include "StoryBase.h"
#include "TrafficBase.h"
#include "AssetBase.h"

#include "map/map.h"
#include "map/terrain.h"
#include "map/roadnet.h"
#include "map/block.h"
#include "map/zone.h"
#include "map/building.h"
#include "populace/populace.h"
#include "society/society.h"
#include "story/story.h"
#include "story/script.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "player/player.h"

#undef UpdateResource


using namespace std;

unordered_map<string, HMODULE> AGlobalBase::modHandles = {};

AGlobalBase::AGlobalBase() :
	map(nullptr),
	populace(nullptr),
	society(nullptr),
	story(nullptr),
	industry(nullptr),
	traffic(nullptr),
	player(nullptr),
	implement(nullptr),
	terrainActor(nullptr),
	roadnetActor(nullptr),
	zoneActor(nullptr),
	buildingActor(nullptr),
	roomActor(nullptr),
	populaceActor(nullptr),
	storyActor(nullptr),
	trafficActor(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

AGlobalBase::~AGlobalBase() {
	if (implement) {
		delete implement;
		implement = nullptr;
	}
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
#if WITH_EDITOR
		for (auto& pluginPath : Config::GetPlugins()) {
			FString pluginFile(UTF8_TO_TCHAR(pluginPath.data()));
			FString pluginDir  = FPaths::GetPath(pluginFile);
			FString pluginName = FPaths::GetBaseFilename(pluginFile);
			FString contentDir = FPaths::ConvertRelativePathToFull(pluginDir / TEXT("Content"));
			FString mountPoint = TEXT("/") + pluginName + TEXT("/");
			if (FPaths::DirectoryExists(contentDir)) {
				FPackageName::RegisterMountPoint(mountPoint, contentDir);
				if (IAssetRegistry* AR = IAssetRegistry::Get()) {
					AR->ScanPathsSynchronous({ mountPoint }, true);
				}
			}
		}
#else
		for (auto& pakPath : Config::GetPakFiles()) {
			FString pakFile(UTF8_TO_TCHAR(pakPath.data()));
			FString pluginName = FPaths::GetBaseFilename(pakFile);
			FString mountPoint = TEXT("/") + pluginName + TEXT("/");
			if (FCoreDelegates::MountPaksEx.IsBound()) {
				TArray<UE::FMountPaksExArgs> mountArgsArr;
				UE::FMountPaksExArgs& mountArgs = mountArgsArr.AddDefaulted_GetRef();
				mountArgs.PakFilePath = *pakFile;
				mountArgs.Order = 4;
				mountArgs.MountOptions.MountFlags = FPakMountOptions::EMountFlags::SkipContainerFile;
				if (FCoreDelegates::MountPaksEx.Execute(mountArgsArr)) {
					FPackageName::RegisterMountPoint(mountPoint, TEXT("../../../") + pluginName + TEXT("/Content/"));
					FShaderCodeLibrary::OpenLibrary(pluginName, TEXT("../../../") + pluginName + TEXT("/Content/"));
					if (IAssetRegistry* AR = IAssetRegistry::Get()) {
						AR->ScanPathsSynchronous({ mountPoint }, true);
					}
				}
			}
		}
#endif

		map = new Map();
		populace = new Populace();
		society = new Society();
		story = new Story();
		industry = new Industry();
		traffic = new Traffic();
		player = new Player();
		implement = new PostImplement(map, populace, society, story, industry, traffic, player);

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
		player->InitAssets(modHandles, mods);
		player->InitPuzzles(modHandles, mods);
		player->InitApps(modHandles, mods);

		int size = 16;

		player->Init();
		map->InitBlocks(size, size);
		traffic->InitBuildings(map);
		int accomodation = map->InitContents();
		populace->Init(accomodation, player, implement);
		map->Checkin(populace, player);
		traffic->InitTraffic(map, populace);
		society->Init(map, populace, player);
		story->Init(map, populace, player);
		industry->Init(map);

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
		populaceActor = GetWorld()->SpawnActor<APopulaceBase>(PopulaceClass, Location, Rotation);
		populaceActor->SetGlobal(this);
		storyActor = GetWorld()->SpawnActor<AStoryBase>(StoryClass, Location, Rotation);
		storyActor->SetGlobal(this);
		trafficActor = GetWorld()->SpawnActor<ATrafficBase>(TrafficClass, Location, Rotation);
		trafficActor->SetGlobal(this);
		assetActor = GetWorld()->SpawnActor<AAssetBase>(AssetClass, Location, Rotation);
		assetActor->SetGlobal(this);

		auto pos = map->GetPlayerPos();
		SetLocation(FVector(pos.first, pos.second, 20.f));
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
	auto applyAndFree = [&](vector<pair<Change*, Script*>> changes) {
		if (!changes.empty() && storyActor && story->GetScript()) {
			vector<function<pair<bool, ValueType>(const string&)>> getValues = {
				[&](const string& v) -> pair<bool, ValueType> {
					return story->GetScript()->GetValue(v);
				}
			};
			storyActor->ApplyChanges(changes, getValues);
		}
		for (auto& [c, s] : changes) delete c;
	};
	applyAndFree(society->Tick(player, story, implement));
	applyAndFree(populace->Tick(map, story, player, implement));
	story->Tick(player);
	industry->Tick(player);
	traffic->Tick(player);
}

Map* AGlobalBase::GetMap() const {
	return map;
}

Populace* AGlobalBase::GetPopulace() const {
	return populace;
}

Society* AGlobalBase::GetSociety() const {
	return society;
}

Story* AGlobalBase::GetStory() const {
	return story;
}

Industry* AGlobalBase::GetIndustry() const {
	return industry;
}

Traffic *AGlobalBase::GetTraffic() const {
	return traffic;
}

Player* AGlobalBase::GetPlayer() const {
	return player;
}

PostImplement* AGlobalBase::GetImplement() const {
	return implement;
}

ATerrainBase* AGlobalBase::GetTerrainActor() const {
	return terrainActor;
}

ARoadnetBase* AGlobalBase::GetRoadnetActor() const {
	return roadnetActor;
}

AZoneBase* AGlobalBase::GetZoneActor() const {
	return zoneActor;
}

ABuildingBase* AGlobalBase::GetBuildingActor() const {
	return buildingActor;
}

ARoomBase* AGlobalBase::GetRoomActor() const {
	return roomActor;
}

APopulaceBase* AGlobalBase::GetPopulaceActor() const {
	return populaceActor;
}

AStoryBase* AGlobalBase::GetStoryActor() const {
	return storyActor;
}

ATrafficBase* AGlobalBase::GetTrafficActor() const {
	return trafficActor;
}

AAssetBase* AGlobalBase::GetAssetActor() const {
	return assetActor;
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

unordered_map<string, uint32> terrainColors;
unordered_map<string, uint32> chunkColors;
void AGlobalBase::DrawMap(UCanvasBuffer* buffer) {
	for (int i = 0; i < map->GetSize().first; i++) {
		for (int j = 0; j < map->GetSize().second; j++) {
			uint32 color = 0;

			auto terrain = map->GetTerrain(i, j);
			if (terrainColors.find(terrain) != terrainColors.end()) {
				color = terrainColors[terrain];
			}
			else {
				color = GetRandom(0x1000000);
				terrainColors[terrain] = color;
			}

			buffer->SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
			buffer->PutPixel(i, j);
		}
	}

	auto roadnet = map->GetRoadnet();

	buffer->SetColor(255, 255, 255);
	for (auto road : roadnet->GetRoads()) {
		float x1 = road->GetPoint(0.f).GetX();
		float y1 = road->GetPoint(0.f).GetY();
		float x2 = road->GetPoint(1.f).GetX();
		float y2 = road->GetPoint(1.f).GetY();
		buffer->PutLine(x1, y1, x2, y2);
	}

	auto blocks = roadnet->GetBlocks();
	for (auto block : blocks) {
		auto zones = block->GetZones();
		for (auto& [zoneName, zone] : zones) {
			vector<pair<float, float>> vertices = {
				block->GetPosition(zone->GetPosX() + zone->GetSizeX() / 2.f, zone->GetPosY() + zone->GetSizeY() / 2.f),
				block->GetPosition(zone->GetPosX() - zone->GetSizeX() / 2.f, zone->GetPosY() + zone->GetSizeY() / 2.f),
				block->GetPosition(zone->GetPosX() - zone->GetSizeX() / 2.f, zone->GetPosY() - zone->GetSizeY() / 2.f),
				block->GetPosition(zone->GetPosX() + zone->GetSizeX() / 2.f, zone->GetPosY() - zone->GetSizeY() / 2.f),
			};

			uint32 color;
			if (chunkColors.find(zoneName) != chunkColors.end()) {
				color = chunkColors[zoneName];
			}
			else {
				color = GetRandom(0x1000000);
				chunkColors[zoneName] = color;
			}

			float x1 = vertices[0].first;
			float y1 = vertices[0].second;
			float x2 = vertices[1].first;
			float y2 = vertices[1].second;
			float x3 = vertices[2].first;
			float y3 = vertices[2].second;
			float x4 = vertices[3].first;
			float y4 = vertices[3].second;

			buffer->SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
			buffer->PutTriangle(x1, y1, x2, y2, x3, y3, true);
			buffer->PutTriangle(x1, y1, x3, y3, x4, y4, true);
		}

		auto buildings = block->GetBuildings();
		for (auto& [buildingName, building] : buildings) {
			vector<pair<float, float>> vertices = {
				block->GetPosition(building->GetPosX() + building->GetSizeX() / 2.f, building->GetPosY() + building->GetSizeY() / 2.f),
				block->GetPosition(building->GetPosX() - building->GetSizeX() / 2.f, building->GetPosY() + building->GetSizeY() / 2.f),
				block->GetPosition(building->GetPosX() - building->GetSizeX() / 2.f, building->GetPosY() - building->GetSizeY() / 2.f),
				block->GetPosition(building->GetPosX() + building->GetSizeX() / 2.f, building->GetPosY() - building->GetSizeY() / 2.f),
			};

			uint32 color;
			if (chunkColors.find(buildingName) != chunkColors.end()) {
				color = chunkColors[buildingName];
			}
			else {
				color = GetRandom(0x1000000);
				chunkColors[buildingName] = color;
			}

			float x1 = vertices[0].first;
			float y1 = vertices[0].second;
			float x2 = vertices[1].first;
			float y2 = vertices[1].second;
			float x3 = vertices[2].first;
			float y3 = vertices[2].second;
			float x4 = vertices[3].first;
			float y4 = vertices[3].second;

			buffer->SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
			buffer->PutTriangle(x1, y1, x2, y2, x3, y3, true);
			buffer->PutTriangle(x1, y1, x3, y3, x4, y4, true);
		}
	}
}

void AGlobalBase::InitPuzzle(FString puzzle, int width, int height) {
	Player::puzzleFactory->InitPuzzle(TCHAR_TO_UTF8(*puzzle), width, height, implement);
}

int32 AGlobalBase::LoopPuzzle(FString puzzle, UCanvasBuffer* canvas, int ms) {
	return (int32)Player::puzzleFactory->LoopPuzzle(TCHAR_TO_UTF8(*puzzle), canvas->GetCanvas(), ms, implement);
}

void AGlobalBase::InitPhone(int width, int height) {
	GetPlayer()->GetPhone()->Init(width, height);
}

int32 AGlobalBase::LoopPhone(UCanvasBuffer* canvas, int ms) {
	return (int32)GetPlayer()->GetPhone()->Loop(canvas->GetCanvas(), ms, implement);
}