#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "RoadnetBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FConnection {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FVector v1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FVector v2;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	float width;
};

UCLASS()
class FOREVER_API ARoadnetBase : public AActor {
	GENERATED_BODY()
	
public:	
	ARoadnetBase();
	~ARoadnetBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);
	void MarkDirty();

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateRoadnet();
	UFUNCTION(BlueprintCallable, Category = "Updating")
	TArray<FConnection> GetRoadnet();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	bool dirty = true;
};
