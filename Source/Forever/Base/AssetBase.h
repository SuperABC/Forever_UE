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
	int32 usage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	FString path;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	FVector location;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Asset")
	float scale;
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
	AActor* GetInstance(FString room, FString name);
	UFUNCTION(BlueprintCallable, Category = "World")
	void AddInstance(FString room, FString name, AActor* actor);
	UFUNCTION(BlueprintCallable, Category = "World")
	void RemoveInstance(FString room, FString name, AActor*& instance);
	UFUNCTION(BlueprintCallable, Category = "World")
	void RemoveInstances(FString room, TArray<AActor*>& instances);

	UFUNCTION(BlueprintCallable, Category = "Asset")
	FAsset GetAsset(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	TArray<FAsset> GetAssets(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	FString PickAsset(FString path, FString target, FString& room, FAsset& outAsset);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	bool DestroyAsset(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	bool DestroyAssets(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	bool DropAsset(FString path, FString& room, FAsset& outAsset);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	bool UseAsset(FString path);
	UFUNCTION(BlueprintCallable, Category = "Asset")
	TArray<FAsset> GetRoom();
	UFUNCTION(BlueprintCallable, Category = "Asset")
	bool SetPosition(FString asset, FVector position);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

private:
	Room* GetCurrentRoom() const;

	std::unordered_map<std::string, TMap<FString, AActor*>> assetInstances;

};
