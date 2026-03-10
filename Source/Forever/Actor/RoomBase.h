#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "RoomBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FRoom {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FString name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector center;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	FVector size;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Building")
	float rotation;
};

UCLASS()
class FOREVER_API ARoomBase : public AActor {
	GENERATED_BODY()
	
public:	
	ARoomBase();
	~ARoomBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);

	void AddBuilding(std::string name, Building* building);
	void RemoveBuilding(std::string name);

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateRoom(const FString& name, const TArray<FRoom>& adds, const TArray<AActor*>& removes);
	UFUNCTION(BlueprintCallable, Category = "World")
	void SetInstance(FString name, AActor* room);
	UFUNCTION(BlueprintCallable, Category = "World")
	void RemoveInstance(FString name);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void EnterRoom(FString room);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void LeaveRoom(FString room);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AActor* global;

private:
	std::unordered_map<std::string, TArray<AActor*>> roomInstances;
};
