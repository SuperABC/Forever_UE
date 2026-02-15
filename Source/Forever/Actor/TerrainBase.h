#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "TerrainBase.generated.h"


UCLASS()
class FOREVER_API ATerrainBase : public AActor {
	GENERATED_BODY()
	
public:	
	ATerrainBase();
	~ATerrainBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);
	void MarkDirty();

	UFUNCTION(BlueprintCallable, Category = "Updating")
	void LookupTerrain(int x, int y, FString &type, float &height);
	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateTerrain(int x1, int y1, int x2, int y2);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	bool dirty = true;
};
