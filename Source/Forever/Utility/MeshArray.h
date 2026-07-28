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

	// 沿有理Bezier曲线按弧长均匀分段，输出每段可直接用于SplineMeshComponent::SetStartAndEnd的起止点、切线及缩放
	UFUNCTION(BlueprintCallable, Category = "MeshArray")
	static bool RepeatMeshAlongCurve(FVector v1, FVector v2, const TArray<FVector>& controls, const TArray<float>& weights,
		FVector primitive, int axis,
		TArray<FVector>& starts, TArray<FVector>& startTangents,
		TArray<FVector>& ends, TArray<FVector>& endTangents, TArray<FVector>& sizes, int& segmentCount);
};