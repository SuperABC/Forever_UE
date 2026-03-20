#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#define NOMINMAX

#include "config.h"

#include "StartBase.generated.h"


UCLASS()
class FOREVER_API AStartBase : public AActor {
	GENERATED_BODY()
	
public:	
	AStartBase();
	~AStartBase();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Config")
	TArray<FString> GetDllPaths();
	UFUNCTION(BlueprintCallable, Category = "Config")
	TArray<FString> GetModPaths();
	UFUNCTION(BlueprintCallable, Category = "Config")
	void AddDllPath(FString path);
	UFUNCTION(BlueprintCallable, Category = "Config")
	void RemoveDllPath(FString path);
	UFUNCTION(BlueprintCallable, Category = "Config")
	void GetChecks(FString type, TArray<FString>& mods, TArray<bool>& enables);
	UFUNCTION(BlueprintCallable, Category = "Config")
	void CheckMod(FString type, FString mod, bool enabled);
	UFUNCTION(BlueprintCallable, Category = "Config")
	TArray<FString> GetResourcePaths();
	UFUNCTION(BlueprintCallable, Category = "Config")
	TArray<FString> GetResources();
	UFUNCTION(BlueprintCallable, Category = "Config")
	void AddResourcePath(FString path);
	UFUNCTION(BlueprintCallable, Category = "Config")
	void RemoveResourcePath(FString path);
	UFUNCTION(BlueprintCallable, Category = "Config")
	FString GetScriptPath();
	UFUNCTION(BlueprintCallable, Category = "Config")
	void SetScriptPath(FString path);

protected:
	virtual void BeginPlay() override;
};
