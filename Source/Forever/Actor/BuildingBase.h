#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "BuildingBase.generated.h"


UCLASS()
class FOREVER_API ABuildingBase : public AActor {
	GENERATED_BODY()
	
public:	
	ABuildingBase();
	~ABuildingBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);
	void MarkDirty();

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateBuilding();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	bool dirty = true;
};
