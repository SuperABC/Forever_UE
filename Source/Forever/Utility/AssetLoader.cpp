#include "AssetLoader.h"


UObject* UAssetLoader::LoadAssetFromPath(const FString& Path, TSubclassOf<UObject> TargetClass) {
	UObject* asset = LoadObject<UObject>(nullptr, *Path);

	if (asset && TargetClass && asset->IsA(TargetClass)) {
		return asset;
	}

	return nullptr;
}

TSubclassOf<APawn> UAssetLoader::LoadPawnFromPath(const FString& Path) {
	UClass* pawn = LoadClass<APawn>(nullptr, *Path);

	if (pawn) {
		return pawn;
	}

	return nullptr;
}