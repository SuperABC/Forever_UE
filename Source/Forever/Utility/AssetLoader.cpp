#include "AssetLoader.h"


UObject* UAssetLoader::LoadAssetFromPath(const FString& Path, TSubclassOf<UObject> TargetClass) {
    UObject* LoadedAsset = LoadObject<UObject>(nullptr, *Path);

    if (LoadedAsset && TargetClass && LoadedAsset->IsA(TargetClass)) {
        return LoadedAsset;
    }

    return nullptr;
}