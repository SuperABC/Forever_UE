#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "ZoneBase.generated.h"


UCLASS()
class FOREVER_API AZoneBase : public AActor {
	GENERATED_BODY()
	
public:	
	AZoneBase();
	~AZoneBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);
	void MarkDirty();

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateZone();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	bool dirty = true;
};
