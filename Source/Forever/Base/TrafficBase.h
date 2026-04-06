#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#define NOMINMAX

#include "common/class.h"

#include "TrafficBase.generated.h"


UCLASS()
class FOREVER_API ATrafficBase : public AActor {
	GENERATED_BODY()
	
public:	
	ATrafficBase();
	~ATrafficBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateTraffic();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AActor* global;
};
