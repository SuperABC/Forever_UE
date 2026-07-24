#pragma once

#define NOMINMAX

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "../Utility/CanvasBuffer.h"

#include "common/class.h"
#include "common/implement.h"

#include <string>
#include <unordered_map>
#include <windows.h>

#include "GlobalBase.generated.h"


class ATerrainBase;
class ARoadnetBase;
class AZoneBase;
class ABuildingBase;
class ARoomBase;
class APopulaceBase;
class AStoryBase;
class ATrafficBase;
class AAssetBase;

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

	Map* GetMap() const;
	Populace* GetPopulace() const;
	Society* GetSociety() const;
	Story* GetStory() const;
	Industry* GetIndustry() const;
	Traffic* GetTraffic() const;
	Player* GetPlayer() const;
	PostImplement* GetImplement() const;

	ATerrainBase* GetTerrainActor() const;
	ARoadnetBase* GetRoadnetActor() const;
	AZoneBase* GetZoneActor() const;
	ABuildingBase* GetBuildingActor() const;
	ARoomBase* GetRoomActor() const;
	APopulaceBase* GetPopulaceActor() const;
	AStoryBase* GetStoryActor() const;
	ATrafficBase* GetTrafficActor() const;
	AAssetBase* GetAssetActor() const;

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
	TSubclassOf<AActor> PopulaceClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> StoryClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> TrafficClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> AssetClass;

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
	UFUNCTION(BlueprintCallable, Category = "World")
	void DrawMap(UCanvasBuffer* buffer);
	UFUNCTION(BlueprintCallable, Category = "World")
	void InitPuzzle(FString puzzle, int width, int height);
	UFUNCTION(BlueprintCallable, Category = "World")
	int32 LoopPuzzle(FString puzzle, UCanvasBuffer* canvas, int ms);
	UFUNCTION(BlueprintCallable, Category = "World")
	void InitPhone(int width, int height);
	UFUNCTION(BlueprintCallable, Category = "World")
	int32 LoopPhone(UCanvasBuffer* canvas, int ms);

protected:
	virtual void BeginPlay() override;

	static std::unordered_map<std::string, HMODULE> modHandles;

	Map* map;
	Populace* populace;
	Society* society;
	Story* story;
	Industry* industry;
	Traffic* traffic;
	Player* player;

	OBJECT_HOLDER PostImplement* implement;

	ATerrainBase* terrainActor;
	ARoadnetBase* roadnetActor;
	AZoneBase* zoneActor;
	ABuildingBase* buildingActor;
	ARoomBase* roomActor;
	APopulaceBase* populaceActor;
	AStoryBase* storyActor;
	ATrafficBase* trafficActor;
	AAssetBase* assetActor;
};
