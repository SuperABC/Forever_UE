#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

#include "TerrainBase.h"
#include "RoadnetBase.h"
#include "ZoneBase.h"
#include "BuildingBase.h"
#include "RoomBase.h"
#include "TrafficBase.h"
#include "PopulaceBase.h"

#include "map/map.h"
#include "populace/populace.h"
#include "society/society.h"
#include "story/story.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "player/player.h"

#include "GlobalBase.generated.h"


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

	UFUNCTION(BlueprintImplementableEvent, Category = "Status")
	void GetLocation(FVector location);

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
	APopulaceBase* populaceActor = nullptr;
	ATrafficBase* trafficActor = nullptr;
};
