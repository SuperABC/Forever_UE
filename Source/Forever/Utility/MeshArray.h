#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MeshArray.generated.h"


UCLASS()
class FOREVER_API UMeshArray : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MeshArray")
	static bool RepeatMesh1D(FVector center, FVector size, FVector primitive, float rotate, int axis,
		TArray<FVector>& centers, TArray<FVector>& sizes);
};