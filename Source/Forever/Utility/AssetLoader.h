#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AssetLoader.generated.h"


UCLASS()
class FOREVER_API UAssetLoader : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AssetLoader", meta = (DeterminesOutputType = "TargetClass"))
	static UObject* LoadAssetFromPath(const FString& Path, TSubclassOf<UObject> TargetClass);
};