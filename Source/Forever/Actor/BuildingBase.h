#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "BuildingBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FWall {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector center;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector size;
};

USTRUCT(Blueprintable, BlueprintType)
struct FBuilding {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FString name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector center;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector size;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	float rotation;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	TArray<FWall> walls;
};

UCLASS()
class FOREVER_API ABuildingBase : public AActor {
	GENERATED_BODY()
	
public:	
	ABuildingBase();
	~ABuildingBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateBuilding(const TArray<FBuilding>& buildings);
	UFUNCTION(BlueprintCallable, Category = "World")
	void SetInstance(FString name, AActor* actor);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AActor* global;

	std::unordered_map<std::string, AActor*> buildingInstances;

private:
	void ConstructBuilding(Building* building, FBuilding& info);
};
