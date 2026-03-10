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

struct FMesh {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector center;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector size;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	float rot;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FString address;
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	TArray<FMesh> meshes;
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

	UFUNCTION(BlueprintCallable, Category = "Story")
	void EnterBuilding(FString building);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void LeaveBuilding(FString building);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AActor* global;

private:
	float GetRotation(FACE_DIRECTION direction);
	void ConstructBuilding(Building* building, FBuilding& info);
	TArray<FWall> ConstructQuad(FVector center, FVector size, std::vector<bool> directions,
		std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> doors,
		std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>> windows);

	std::unordered_map<std::string, AActor*> buildingInstances;
};
