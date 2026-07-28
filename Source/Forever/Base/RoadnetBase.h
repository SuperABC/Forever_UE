#pragma once

#define NOMINMAX

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GlobalBase.h"

#include "common/class.h"

#include "RoadnetBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FRoad {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	FVector v1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	FVector v2;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	FString type;
	// v1处的切线方向（非单位向量），供SplineMeshComponent等曲线绘制使用
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	FVector tangent1;
	// v2处的切线方向（非单位向量），供SplineMeshComponent等曲线绘制使用
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	FVector tangent2;
	// 控制点坐标（v1、v2之间，按顺序排列）
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	TArray<FVector> controls;
	// 控制点对应权重（与controls一一对应）
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Road")
	TArray<float> weights;
};

USTRUCT(Blueprintable, BlueprintType)
struct FNode {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Node")
	FVector pos;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Node")
	FString category;
};

USTRUCT(Blueprintable, BlueprintType)
struct FConnection {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FVector start;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Connection")
	FVector end;
};

UCLASS()
class FOREVER_API ARoadnetBase : public AActor {
	GENERATED_BODY()
	
public:	
	ARoadnetBase();
	~ARoadnetBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AGlobalBase* g);

	void MarkDirty();

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateRoadnet();
	UFUNCTION(BlueprintCallable, Category = "World")
	TArray<FRoad> GetRoadnet();

	UFUNCTION(BlueprintCallable, Category = "World")
	void GetNavigations(TArray<FNode>& nodes, TArray<FConnection>& connections);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

	bool dirty;
};
