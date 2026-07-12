#pragma once

#define NOMINMAX

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GlobalBase.h"

#include "common/class.h"

#include "TrafficBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FVehicle {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
	FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
	FString path;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
	FVector pos;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
	float rotation;
};

UCLASS()
class FOREVER_API ATrafficBase : public AActor {
	GENERATED_BODY()
	
public:	
	ATrafficBase();
	~ATrafficBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AGlobalBase* g);

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateTraffic(const TArray<FVehicle>& adds, const TArray<FString>& removes);
	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void EnterVehicle(const FString& vehicle);
	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void LeaveVehicle(const FString& vehicle);
	UFUNCTION(BlueprintCallable, Category = "World")
	AActor* GetInstance(FString name);
	UFUNCTION(BlueprintCallable, Category = "World")
	void AddInstance(FString name, AActor* actor);
	UFUNCTION(BlueprintCallable, Category = "World")
	void RemoveInstance(FString name, AActor*& instances);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

	std::unordered_map<std::string, AActor*> vehicleInstances;
};
