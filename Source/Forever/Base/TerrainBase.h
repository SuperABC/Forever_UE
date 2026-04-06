#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#define NOMINMAX

#include "common/class.h"

#include "TerrainBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FCoordinate {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IntPair")
	int32 x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IntPair")
	int32 y;
};

UCLASS()
class FOREVER_API ATerrainBase : public AActor {
	GENERATED_BODY()
	
public:	
	ATerrainBase();
	~ATerrainBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);

	void InitInstances(int width, int height);

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateTerrain(const TArray<FCoordinate> &adds, const TArray<FCoordinate>& removes);
	UFUNCTION(BlueprintCallable, Category = "World")
	void LookupTerrain(int x, int y, FString &type, float &height);
	UFUNCTION(BlueprintCallable, Category = "World")
	void SetInstance(int x, int y, TArray<int> ids);
	UFUNCTION(BlueprintCallable, Category = "World")
	void RemoveInstance(int x, int y, TArray<int>& ids);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AActor* global;

	std::vector<std::vector<TArray<int>>> terrainInstances;
	std::vector<std::pair<int, int>> idList;
};
