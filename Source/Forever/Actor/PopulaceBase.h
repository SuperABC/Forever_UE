#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "populace/populace.h"

#include "PopulaceBase.generated.h"


UCLASS()
class FOREVER_API APopulaceBase : public AActor {
	GENERATED_BODY()
	
public:	
	APopulaceBase();
	~APopulaceBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);
	void MarkDirty();

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdatePopulace();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	bool dirty = true;
};
