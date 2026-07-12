#pragma once

#define NOMINMAX

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DArray.h"

#include "GlobalBase.h"

#include "common/class.h"

#include "TerrainBase.generated.h"

class UProceduralMeshComponent;


USTRUCT(BlueprintType)
struct FRect2D {
	GENERATED_BODY()

	// 矩形中心（单位：地形格，1格=10m）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	FVector2D Center;

	// 矩形尺寸（单位：地形格）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	FVector2D Size;
};

USTRUCT(BlueprintType)
struct FTri2D {
	GENERATED_BODY()

	// 直角顶点坐标（单位：地形格，相对于格子左下角）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	FVector2D Corner;

	// x轴边长（有符号，正为+X方向）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	float SizeX = 0.f;

	// y轴边长（有符号，正为+Y方向）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	float SizeY = 0.f;

};

USTRUCT(Blueprintable, BlueprintType)
struct FCoordinate {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IntPair")
	int32 x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IntPair")
	int32 y;
};

// 精细地形混合单元：权重最高的4个地形id及其权重
struct FFineCell {
	uint8 Ids[4] = { 0, 0, 0, 0 };
	float Weights[4] = { 0.f, 0.f, 0.f, 0.f };
};

UCLASS()
class FOREVER_API ATerrainBase : public AActor {
	GENERATED_BODY()
	
public:	
	ATerrainBase();
	~ATerrainBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AGlobalBase* g);

	void InitInstances(int width, int height);

	UFUNCTION(BlueprintImplementableEvent, Category = "World")
	void UpdateTerrain(const TArray<FCoordinate> &adds, const TArray<FCoordinate>& removes);
	UFUNCTION(BlueprintCallable, Category = "World")
	void LookupTerrain(int elemX, int elemY, FString &type, float &height,
		TArray<FRect2D> &rects, TArray<FTri2D> &tris);
	UFUNCTION(BlueprintCallable, Category = "World")
	void SetInstance(int elemX, int elemY, TArray<int> ids);
	UFUNCTION(BlueprintCallable, Category = "World")
	void RemoveInstance(int elemX, int elemY, TArray<int>& ids);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

	UPROPERTY()
	UMaterialInstanceDynamic* terrainMaterial = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTexture2DArray* terrainDiffuseArray = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTexture2D* terrainIndexMap = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* fineMateria0 = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* fineMaterial1 = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* fineMaterial2 = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTexture2D* fineIndexMap0 = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTexture2D* finePowerMap0 = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTexture2D* fineIndexMap1 = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTexture2D* finePowerMap1 = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTexture2D* fineIndexMap2 = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTexture2D* finePowerMap2 = nullptr;

	UPROPERTY()
	TArray<UProceduralMeshComponent*> gridMeshes;

	UPROPERTY()
	UMaterialInstanceDynamic* oceanMaterial = nullptr;

	UPROPERTY()
	UProceduralMeshComponent* oceanMesh = nullptr;

	std::vector<std::pair<std::pair<int, int>, float>> currentPivots;

	std::vector<std::vector<TArray<int>>> terrainInstances;
	std::vector<std::pair<int, int>> idList;

private:
	void BuildLevel(int levelIdx, std::pair<int, int> pos, float size, int childPos);

	// 构建覆盖所有"ocean"地形格的海面网格，高度固定为0
	void BuildOceanMesh(int width, int height);

	// 将输入网格的地形混合数据下采样（分辨率翻倍）到输出网格
	void DownsampleFine(const TArray<FFineCell>& in, int32 inRes, TArray<FFineCell>& out) const;
};
