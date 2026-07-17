#pragma once

#define NOMINMAX

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GlobalBase.h"

#include "common/class.h"

#include "AssetBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FAsset {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	FString name;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	FString type;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	FString icon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	FString mesh;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	bool isContainer;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	bool usable;
};

UCLASS()
class FOREVER_API AAssetBase : public AActor {
	GENERATED_BODY()

public:
	AAssetBase();
	~AAssetBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AGlobalBase* g);

	UFUNCTION(BlueprintCallable, Category = "World")
	AActor* GetInstance(FString name);
	UFUNCTION(BlueprintCallable, Category = "World")
	void AddInstance(FString name, AActor* actor);
	UFUNCTION(BlueprintCallable, Category = "World")
	void RemoveInstance(FString name, AActor*& instance);

	UFUNCTION(BlueprintCallable, Category = "Asset")
	FAsset GetAsset(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	TArray<FAsset> GetAssets(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	FString PickAsset(FString path, FString target, FAsset& outAsset);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	bool DestroyAsset(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	bool DestroyAssets(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	bool DropAsset(FString path);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

	std::unordered_map<std::string, AActor*> assetInstances;

private:
	Room* GetCurrentRoom() const;

};
