#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

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

	void InitInstances(int width, int height);
	void SetGlobal(AActor* g);

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateTerrain(const TArray<FCoordinate> &coordinates);
	UFUNCTION(BlueprintCallable, Category = "Updating")
	void LookupTerrain(int x, int y, FString &type, float &height);
	UFUNCTION(BlueprintCallable, Category = "Updating")
	void SetInstance(int x, int y, int id);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	std::vector<std::vector<int>> terrainInstances;
};
