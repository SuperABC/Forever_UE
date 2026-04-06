#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#define NOMINMAX

#include "TerrainBase.h"
#include "RoadnetBase.h"
#include "ZoneBase.h"
#include "BuildingBase.h"
#include "RoomBase.h"
#include "TrafficBase.h"
#include "PopulaceBase.h"
#include "StoryBase.h"

#include "map/map.h"
#include "populace/populace.h"
#include "society/society.h"
#include "story/story.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "player/player.h"

#include <string>
#include <unordered_map>
#include <windows.h>

#include "GlobalBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FStatus {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Global")
	FString time;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Global")
	float chronode;
};

UCLASS()
class FOREVER_API AGlobalBase : public AActor {
	GENERATED_BODY()
	
public:	
	AGlobalBase();
	~AGlobalBase();

	virtual void Tick(float DeltaTime) override;

	Map* GetMap();
	Populace* GetPopulace();
	Society* GetSociety();
	Story* GetStory();
	Industry* GetIndustry();
	Traffic* GetTraffic();
	Player* GetPlayer();

	ATerrainBase* GetTerrainActor();
	ARoadnetBase* GetRoadnetActor();
	AZoneBase* GetZoneActor();
	ABuildingBase* GetBuildingActor();
	ARoomBase* GetRoomActor();
	ATrafficBase* GetTrafficActor();
	APopulaceBase* GetPopulaceActor();
	AStoryBase* GetStoryActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> TerrainClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> RoadnetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> ZoneClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> BuildingClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> RoomClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> TrafficClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> PopulaceClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> StoryClass;

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void GetLocation(FVector& location);
	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void SetLocation(FVector location);
	UFUNCTION(BlueprintCallable, Category = "World")
	void GlobalPause();
	UFUNCTION(BlueprintCallable, Category = "World")
	void GlobalResume();
	UFUNCTION(BlueprintCallable, Category = "World")
	FStatus GetStatus();

protected:
	virtual void BeginPlay() override;

	static std::unordered_map<std::string, HMODULE> modHandles;

	Map* map = nullptr;
	Populace* populace = nullptr;
	Society* society = nullptr;
	Story* story = nullptr;
	Industry* industry = nullptr;
	Traffic* traffic = nullptr;
	Player* player = nullptr;

	ATerrainBase* terrainActor = nullptr;
	ARoadnetBase* roadnetActor = nullptr;
	AZoneBase* zoneActor = nullptr;
	ABuildingBase* buildingActor = nullptr;
	ARoomBase* roomActor = nullptr;
	ATrafficBase* trafficActor = nullptr;
	APopulaceBase* populaceActor = nullptr;
	AStoryBase* storyActor = nullptr;
};
