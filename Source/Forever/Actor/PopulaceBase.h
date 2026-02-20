#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "populace/populace.h"

#include "PopulaceBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FPerson {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FString name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FVector pos;
};

UCLASS()
class FOREVER_API APopulaceBase : public AActor {
	GENERATED_BODY()
	
public:	
	APopulaceBase();
	~APopulaceBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AActor* g);

	UFUNCTION(BlueprintImplementableEvent, Category = "Updating")
	void UpdatePopulace(const TArray<FPerson>& adds, const TMap<FString, AActor*>& removes);
	UFUNCTION(BlueprintCallable, Category = "Updating")
	void AddInstance(FString name, AActor* actor);
	UFUNCTION(BlueprintCallable, Category = "Updating")
	void RemoveInstance(FString name);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Global")
	AActor* global;

	std::unordered_map<std::string, AActor*> personInstances;
};
