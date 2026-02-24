#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetLoader.generated.h"


UCLASS()
class FOREVER_API UAssetLoader : public UBlueprintFunctionLibrary {
    GENERATED_BODY()

public:
    /**
     * 通过完整路径加载一个UObject资产（如纹理、材质、静态网格等）
     * @param Path 资产的完整路径，例如 "/Game/Textures/ganyu.ganyu"
     * @param TargetClass 期望加载的资产类型，用于输出引脚类型推导
     * @return 加载成功的资产对象，失败返回 nullptr
     */
    UFUNCTION(BlueprintCallable, Category = "AssetLoader", meta = (DeterminesOutputType = "TargetClass"))
    static UObject* LoadAssetFromPath(const FString& Path, TSubclassOf<UObject> TargetClass);
};