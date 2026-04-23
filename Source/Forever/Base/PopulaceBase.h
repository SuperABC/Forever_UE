#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#define NOMINMAX

#include "common/class.h"

#include "PopulaceBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FPerson {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
	FString name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
	FString avatar;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
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

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdatePopulace(const TArray<FPerson>& adds, const TArray<FString>& removes);
	UFUNCTION(BlueprintCallable, Category = "World")
	void AddInstance(FString name, AActor* actor);
	UFUNCTION(BlueprintCallable, Category = "World")
	void RemoveInstance(FString name, AActor*& instances);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AActor* global;

	std::unordered_map<std::string, AActor*> personInstances;
};
