#pragma once

#define NOMINMAX

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GlobalBase.h"

#include "common/class.h"

#include "RoadnetBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FRoad {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	FVector v1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	FVector v2;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	FString type;
};

UCLASS()
class FOREVER_API ARoadnetBase : public AActor {
	GENERATED_BODY()
	
public:	
	ARoadnetBase();
	~ARoadnetBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AGlobalBase* g);

	void MarkDirty();

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateRoadnet();
	UFUNCTION(BlueprintCallable, Category = "World")
	TArray<FRoad> GetRoadnet();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

	bool dirty;
};
