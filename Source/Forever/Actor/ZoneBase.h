#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "ZoneBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FZone {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FString name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FVector center;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FVector size;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	float rotation;
};

UCLASS()
class FOREVER_API AZoneBase : public AActor {
	GENERATED_BODY()
	
public:	
	AZoneBase();
	~AZoneBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateZone(const TArray<FZone>& zones);
	UFUNCTION(BlueprintCallable, Category = "Updating")
	void SetInstance(FString name, AActor* actor);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	std::unordered_map<std::string, AActor*> zoneInstances;
};
