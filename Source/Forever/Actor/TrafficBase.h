#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "TrafficBase.generated.h"


UCLASS()
class FOREVER_API ATrafficBase : public AActor {
	GENERATED_BODY()
	
public:	
	ATrafficBase();
	~ATrafficBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);
	void MarkDirty();

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateTraffic();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	bool dirty = true;
};
