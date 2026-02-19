#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "map/map.h"

#include "RoomBase.generated.h"


UCLASS()
class FOREVER_API ARoomBase : public AActor {
	GENERATED_BODY()
	
public:	
	ARoomBase();
	~ARoomBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdateRoom();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;
};
