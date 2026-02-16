#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "RoadnetBase.generated.h"


UCLASS()
class FOREVER_API ARoadnetBase : public AActor {
	GENERATED_BODY()
	
public:	
	ARoadnetBase();
	~ARoadnetBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);
	void MarkDirty();

	UFUNCTION(BlueprintCallable, Category = "Updating")
	void GetRoadnet();
	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateRoadnet();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	bool dirty = true;
};
