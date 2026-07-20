#pragma once

#define NOMINMAX

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GlobalBase.h"

#include "common/class.h"

#include "BuildingBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FWall {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector center;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector size;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FString texture;
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
struct FElevator {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FString name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector center;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector size;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	float rot;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	int minFloor;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	int maxFloor;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	int currentFloor;
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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	TArray<FElevator> elevators;
};

UCLASS()
class FOREVER_API ABuildingBase : public AActor {
	GENERATED_BODY()
	
public:	
	ABuildingBase();
	~ABuildingBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AGlobalBase* g);

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateBuilding(const TArray<FBuilding>& buildings);
	UFUNCTION(BlueprintCallable, Category = "World")
	void SetInstance(FString name, AActor* actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void LaunchElevator(AActor* building, const FString& elevator, int target);
	UFUNCTION(BlueprintCallable, Category = "Story")
	AActor* GetInstance(FString name);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void EnterBuilding(FString building);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void LeaveBuilding(FString building);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

private:
	float GetRotation(FACE_DIRECTION direction);
	void ConstructBuilding(Building* building, FBuilding& info);
	void ConstructQuad(FVector center, FVector size, const std::vector<bool>& directions,
		const std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>>& doors,
		const std::unordered_map<FACE_DIRECTION, std::vector<std::pair<std::vector<float>, Quad>>>& windows,
		const FString& wallMaterial, TArray<FWall>& walls, TArray<FMesh>& meshes);

	UPROPERTY()
	TMap<FString, AActor*> buildingInstances;
};
