#include "TerrainBase.h"

#include "GlobalBase.h"
#include "Utility/AssetLoader.h"

#include "map/map.h"
#include "map/geometry.h"
#include "map/block.h"
#include "map/terrain.h"


#ifdef UpdateResource
#undef UpdateResource
#endif

#define DIFFUSE_TEX_SIZE 2048
#define HEIGHT_EPSILON 0.002
using namespace std;

ATerrainBase::ATerrainBase() {
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* sceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = sceneRoot;
}

ATerrainBase::~ATerrainBase() {}

void ATerrainBase::BeginPlay() {
	Super::BeginPlay();
}

void ATerrainBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector location = FVector(0.f, 0.f, 0.f);
	global->GetLocation(location);
	location /= 1000.f;

	auto mapSize = global->GetMap()->GetSize();
	TArray<FCoordinate> adds;
	for (auto y = FMath::Clamp(int(location.Y - 6), 0, mapSize.second - 1); y <= FMath::Clamp(int(location.Y + 6), 0, mapSize.second - 1); y++)
		for (auto x = FMath::Clamp(int(location.X - 6), 0, mapSize.first - 1); x <= FMath::Clamp(int(location.X + 6), 0, mapSize.first - 1); x++)
			if (terrainInstances[y][x].Num() == 0)
				adds.Add(FCoordinate(x, y));

	TArray<FCoordinate> removes;
	for (int y = 0; y < (int)terrainInstances.size(); y++)
		for (int x = 0; x < (int)terrainInstances[y].size(); x++)
			if (terrainInstances[y][x].Num() > 0)
				if (y < FMath::Clamp(int(location.Y - 6), 0, mapSize.second - 1) || y > FMath::Clamp(int(location.Y + 6), 0, mapSize.second - 1) ||
					x < FMath::Clamp(int(location.X - 6), 0, mapSize.first - 1) || x > FMath::Clamp(int(location.X + 6), 0, mapSize.first - 1))
					removes.Add(FCoordinate(x, y));

	UpdateTerrain(adds, removes);

	std::vector<std::pair<int, int>> newPivots(currentPivots.size());
	for (int i = 0; i < currentPivots.size(); i++) {
		auto half = currentPivots[i].second / 2.f;
		int rawX = static_cast<int>((location.X + half / 2.f) / half);
		int rawY = static_cast<int>((location.Y + half / 2.f) / half);

		// 限制 pivot 范围，避免 mesh 覆盖区域越出地图边界
		int minPivot  = 1;
		int maxPivotX = FMath::Max(minPivot, FMath::FloorToInt(mapSize.first  / half) - 1);
		int maxPivotY = FMath::Max(minPivot, FMath::FloorToInt(mapSize.second / half) - 1);

		newPivots[i].first  = FMath::Clamp(rawX, minPivot, maxPivotX);
		newPivots[i].second = FMath::Clamp(rawY, minPivot, maxPivotY);
	}
	bool childChange = false;
	for (int i = 0; i < currentPivots.size(); i++) {
		if (newPivots[i] != currentPivots[i].first || childChange) {
			int childPos = 0;
			if (i >= 1) {
				int idx = (newPivots[i - 1].second + 1 - newPivots[i].second * 2) * 3 +
					(newPivots[i - 1].first + 1 - newPivots[i].first * 2);
				switch (idx) {
				case 0: childPos = 0xCC00; break;
				case 1: childPos = 0x6600; break;
				case 2: childPos = 0x3300; break;
				case 3: childPos = 0x0CC0; break;
				case 4: childPos = 0x0660; break;
				case 5: childPos = 0x0330; break;
				case 6: childPos = 0x00CC; break;
				case 7: childPos = 0x0066; break;
				case 8: childPos = 0x0033; break;
				default:
					break;
				}
			}
			BuildLevel(i, newPivots[i], currentPivots[i].second, childPos);
			if (newPivots[i] != currentPivots[i].first) {
				currentPivots[i].first = newPivots[i];
				childChange = true;
			}
			else {
				childChange = false;
			}
		}
	}
}

void ATerrainBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

void ATerrainBase::InitInstances(int width, int height) {
	terrainInstances = vector<vector<TArray<int>>>(height, vector<TArray<int>>(width));

	Map* map = global->GetMap();
	if (!map) return;

	auto size = map->GetSize();
	auto target = max(size.first, size.second);
	int num = log2(target) - 1;
	currentPivots.resize(num);
	for (int i = 0; i < num; i++) {
		currentPivots[i] = { { INT_MIN, INT_MIN }, 4.f * powf(2, i) };
	}

	const auto& terrainTextures = map->GetTerrainTextures();
	if (terrainTextures.empty()) return;

	int32 numSlices = (int32)terrainTextures.size();

	TArray<UTexture2D*> sourceTextures;
	sourceTextures.SetNum(numSlices);
	for (auto& [textureName, textureInfo] : terrainTextures)
		if (sourceTextures.IsValidIndex(textureInfo.first) && !textureInfo.second.empty())
			sourceTextures[textureInfo.first] = Cast<UTexture2D>(
				UAssetLoader::LoadAssetFromPath(FString(textureInfo.second.c_str()), UTexture2D::StaticClass()));

	// 构建 Texture2DArray
	terrainDiffuseArray = NewObject<UTexture2DArray>(this, NAME_None, RF_Transient);
	terrainDiffuseArray->Filter = TF_Bilinear;
	terrainDiffuseArray->SRGB = true;
	terrainDiffuseArray->MipGenSettings = TMGS_NoMipmaps;
	terrainDiffuseArray->NeverStream = true;

	FTexturePlatformData* platformData = new FTexturePlatformData();
	platformData->SizeX = DIFFUSE_TEX_SIZE;
	platformData->SizeY = DIFFUSE_TEX_SIZE;
	platformData->SetNumSlices(numSlices);
	platformData->PixelFormat = PF_B8G8R8A8;

	FTexture2DMipMap* mip = new FTexture2DMipMap();
	mip->SizeX = DIFFUSE_TEX_SIZE;
	mip->SizeY = DIFFUSE_TEX_SIZE;
	mip->SizeZ = numSlices;
	int32 sliceBytes = DIFFUSE_TEX_SIZE * DIFFUSE_TEX_SIZE * 4;

	mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* destPixels = (uint8*)mip->BulkData.Realloc((int64)sliceBytes * numSlices);
	FMemory::Memzero(destPixels, (int64)sliceBytes * numSlices);

	for (int32 sliceIndex = 0; sliceIndex < sourceTextures.Num(); sliceIndex++) {
		UTexture2D* srcTexture = sourceTextures[sliceIndex];
		if (!srcTexture) continue;
		TArray64<uint8> srcBytes;
		if (srcTexture->Source.GetMipData(srcBytes, 0) && srcBytes.Num() >= sliceBytes)
			FMemory::Memcpy(destPixels + (int64)sliceIndex * sliceBytes, srcBytes.GetData(), sliceBytes);
	}

	mip->BulkData.Unlock();
	platformData->Mips.Add(mip);
	terrainDiffuseArray->SetPlatformData(platformData);
	terrainDiffuseArray->UpdateResource();

	// 构建地形索引图：每像素存储该元素的地形类型槽位索引（PF_G8）
	auto [mapWidth, mapHeight] = map->GetSize();
	terrainIndexMap = UTexture2D::CreateTransient(mapWidth, mapHeight, PF_G8, TEXT("TerrainIndexMap"));
	terrainIndexMap->NeverStream = true;
	terrainIndexMap->Filter = TF_Nearest;

	FTexture2DMipMap& indexMip = terrainIndexMap->GetPlatformData()->Mips[0];
	uint8* indexPixels = (uint8*)indexMip.BulkData.Lock(LOCK_READ_WRITE);
	for (int y = 0; y < mapHeight; y++)
		for (int x = 0; x < mapWidth; x++) {
			const std::string& terrainName = map->GetTerrain(x, y);
			uint8 terrainIndex = 0;
			auto terrainIter = terrainTextures.find(terrainName);
			if (terrainIter != terrainTextures.end())
				terrainIndex = (uint8)FMath::Min(terrainIter->second.first, 255);
			indexPixels[y * mapWidth + x] = terrainIndex;
		}
	indexMip.BulkData.Unlock();
	terrainIndexMap->UpdateResource();

	// Load material and bind textures
	UMaterialInterface* baseMat = Cast<UMaterialInterface>(
		UAssetLoader::LoadAssetFromPath(TEXT("/Game/Asset/Materials/TerrainTemplate.TerrainTemplate"), UMaterialInterface::StaticClass()));
	if (baseMat) {
		terrainMaterial = UMaterialInstanceDynamic::Create(baseMat, this);
		terrainMaterial->SetTextureParameterValue(TEXT("TerrainDiffuseArray"), terrainDiffuseArray);
		terrainMaterial->SetTextureParameterValue(TEXT("TerrainIndexMap"), terrainIndexMap);
	}

	UMaterialInterface* fineBaseMat = Cast<UMaterialInterface>(
		UAssetLoader::LoadAssetFromPath(TEXT("/Game/Asset/Materials/FineTemplate.FineTemplate"), UMaterialInterface::StaticClass()));
	if (fineBaseMat) {
		fineMateria0 = UMaterialInstanceDynamic::Create(fineBaseMat, this);
		fineMaterial1 = UMaterialInstanceDynamic::Create(fineBaseMat, this);
		fineMaterial2 = UMaterialInstanceDynamic::Create(fineBaseMat, this);
	}

	gridMeshes.SetNum(num);
	for (int i = 0; i < num; i++) {
		UProceduralMeshComponent* comp = NewObject<UProceduralMeshComponent>(
			this, *FString::Printf(TEXT("TerrainLod_%d"), i));
		comp->SetupAttachment(RootComponent);
		comp->RegisterComponent();
		AddInstanceComponent(comp);
		gridMeshes[i] = comp;
	}

	BuildOceanMesh(mapWidth, mapHeight);
}

void ATerrainBase::LookupTerrain(int elemX, int elemY, FString& type, float& height,
	TArray<FRect2D>& rects, TArray<FTri2D>& tris) {
	Map* map = global->GetMap();
	if (!map) return;

	type = FString(map->GetTerrain(elemX, elemY).data());
	if (type != "construction")return;

	height = map->GetHeight(elemX, elemY);
	rects.Empty();
	tris.Empty();

	// 局部坐标矩形，初始覆盖整个单元格 [0,1]×[0,1]
	struct RectRegion { float left, right, bottom, top; };
	TArray<RectRegion> work;
	work.Add({ 0.f, 1.f, 0.f, 1.f });

	for (auto& [quad, rotation] : map->GetHatches(elemX, elemY)) {
		float hatchCenterX = quad.GetPosX() - elemX;
		float hatchCenterY = quad.GetPosY() - elemY;
		float halfSizeX = quad.GetSizeX() * 0.5f;
		float halfSizeY = quad.GetSizeY() * 0.5f;

		float cosRot = FMath::Cos(rotation), sinRot = FMath::Sin(rotation);
		// 旋转矩形的 AABB 半尺寸
		float aabbHalfX = FMath::Abs(halfSizeX * cosRot) + FMath::Abs(halfSizeY * sinRot);
		float aabbHalfY = FMath::Abs(halfSizeX * sinRot) + FMath::Abs(halfSizeY * cosRot);

		// AABB 裁剪到单元格内
		float aabbLeft = FMath::Max(hatchCenterX - aabbHalfX, 0.f);
		float aabbRight = FMath::Min(hatchCenterX + aabbHalfX, 1.f);
		float aabbBottom = FMath::Max(hatchCenterY - aabbHalfY, 0.f);
		float aabbTop = FMath::Min(hatchCenterY + aabbHalfY, 1.f);
		if (aabbLeft >= aabbRight || aabbBottom >= aabbTop) continue;

		// 用 T 形分割从现有矩形列表中剔除该 AABB
		TArray<RectRegion> next;
		for (auto& workRect : work) {
			float overlapLeft = FMath::Max(workRect.left, aabbLeft);
			float overlapRight = FMath::Min(workRect.right, aabbRight);
			float overlapBottom = FMath::Max(workRect.bottom, aabbBottom);
			float overlapTop = FMath::Min(workRect.top, aabbTop);
			if (overlapLeft >= overlapRight || overlapBottom >= overlapTop) { next.Add(workRect); continue; }
			if (overlapBottom > workRect.bottom) next.Add({ workRect.left, workRect.right, workRect.bottom, overlapBottom });
			if (overlapTop < workRect.top) next.Add({ workRect.left, workRect.right, overlapTop, workRect.top });
			if (overlapLeft > workRect.left) next.Add({ workRect.left, overlapLeft, overlapBottom, overlapTop });
			if (overlapRight < workRect.right) next.Add({ overlapRight, workRect.right, overlapBottom, overlapTop });
		}
		work = next;

		// 角三角只在包含图案中心的格子中生成
		if ((int)quad.GetPosX() != elemX || (int)quad.GetPosY() != elemY) continue;

		// 旋转矩形的四个顶点（局部坐标）
		FVector2D rectVerts[4] = {
			{ hatchCenterX + halfSizeX * cosRot - halfSizeY * sinRot, hatchCenterY + halfSizeX * sinRot + halfSizeY * cosRot },
			{ hatchCenterX - halfSizeX * cosRot - halfSizeY * sinRot, hatchCenterY - halfSizeX * sinRot + halfSizeY * cosRot },
			{ hatchCenterX - halfSizeX * cosRot + halfSizeY * sinRot, hatchCenterY - halfSizeX * sinRot - halfSizeY * cosRot },
			{ hatchCenterX + halfSizeX * cosRot + halfSizeY * sinRot, hatchCenterY + halfSizeX * sinRot - halfSizeY * cosRot },
		};

		// 确定各顶点对应 AABB 的哪条边
		int idxTop = 0, idxBottom = 0, idxLeft = 0, idxRight = 0;
		for (int vertIdx = 1; vertIdx < 4; vertIdx++) {
			if (rectVerts[vertIdx].Y > rectVerts[idxTop].Y) idxTop = vertIdx;
			if (rectVerts[vertIdx].Y < rectVerts[idxBottom].Y) idxBottom = vertIdx;
			if (rectVerts[vertIdx].X < rectVerts[idxLeft].X) idxLeft = vertIdx;
			if (rectVerts[vertIdx].X > rectVerts[idxRight].X) idxRight = vertIdx;
		}

		float xMin = hatchCenterX - aabbHalfX, xMax = hatchCenterX + aabbHalfX;
		float yMin = hatchCenterY - aabbHalfY, yMax = hatchCenterY + aabbHalfY;

		// 逆时针角三角（图案跨格时可能超出 [0,1]）
		tris.Add({ { xMin, yMax }, (float)(rectVerts[idxTop].X - xMin), (float)(rectVerts[idxLeft].Y - yMax) });
		tris.Add({ { xMax, yMax }, (float)(rectVerts[idxTop].X - xMax), (float)(rectVerts[idxRight].Y - yMax) });
		tris.Add({ { xMax, yMin }, (float)(rectVerts[idxBottom].X - xMax), (float)(rectVerts[idxRight].Y - yMin) });
		tris.Add({ { xMin, yMin }, (float)(rectVerts[idxBottom].X - xMin), (float)(rectVerts[idxLeft].Y - yMin) });
	}

	// 转换为输出矩形
	for (auto& workRect : work) {
		rects.Add({
			FVector2D((workRect.left + workRect.right) * 0.5f, (workRect.bottom + workRect.top) * 0.5f),
			FVector2D(workRect.right - workRect.left, workRect.top - workRect.bottom)
		});
	}
}

void ATerrainBase::SetInstance(int elemX, int elemY, TArray<int> ids) {
	auto mapSize = global->GetMap()->GetSize();
	if (elemX < 0 || elemY < 0 || elemX >= mapSize.first || elemY >= mapSize.second)
		return;

	terrainInstances[elemY][elemX] = ids;
	for (int instanceCount = 0; instanceCount < ids.Num(); ++instanceCount)
		idList.push_back({ elemX, elemY });
}

void ATerrainBase::RemoveInstance(int elemX, int elemY, TArray<int>& ids) {
	auto mapSize = global->GetMap()->GetSize();
	if (elemX < 0 || elemY < 0 || elemX >= mapSize.first || elemY >= mapSize.second) return;

	ids = terrainInstances[elemY][elemX];
	if (ids.Num() == 0) return;

	ids.Sort([](int lhs, int rhs) { return lhs > rhs; });

	for (int removedId : ids) {
		terrainInstances[elemY][elemX].Remove(removedId);
		idList.erase(idList.begin() + removedId);
		for (auto& instanceRow : terrainInstances)
			for (auto& cellIds : instanceRow)
				for (int& instanceId : cellIds)
					if (instanceId > removedId)
						instanceId--;
	}
}

void ATerrainBase::BuildLevel(int levelIdx, pair<int, int> pos, float size, int childPos) {
	Map* map = global->GetMap();
	if (!map) return;

	auto mapSize = map->GetSize();

	UProceduralMeshComponent* mesh = gridMeshes[levelIdx];

	const float cellSize   = size / 32.f;
	const float startX     = pos.first  * (size / 2.f) - size / 2.f;
	const float startY     = pos.second * (size / 2.f) - size / 2.f;
	const float worldScale = 1000.f;

	TArray<FVector>   vertices;
	TArray<int32>     triangles;
	TArray<FVector2D> uvs;

	vertices.Reserve(33 * 33);
	uvs.Reserve(33 * 33);

	for (int vy = 0; vy <= 32; vy++) {
		for (int vx = 0; vx <= 32; vx++) {
			float mapX = startX + vx * cellSize;
			float mapY = startY + vy * cellSize;

			int ex0 = FMath::Clamp(FMath::FloorToInt(mapX), 0, mapSize.first  - 1);
			int ey0 = FMath::Clamp(FMath::FloorToInt(mapY), 0, mapSize.second - 1);
			int ex1 = FMath::Clamp(ex0 + 1,                 0, mapSize.first  - 1);
			int ey1 = FMath::Clamp(ey0 + 1,                 0, mapSize.second - 1);
			float tx = mapX - FMath::FloorToInt(mapX);
			float ty = mapY - FMath::FloorToInt(mapY);

			float h = map->GetHeight(ex0, ey0) * (1-tx) * (1-ty)
			        + map->GetHeight(ex1, ey0) *    tx  * (1-ty)
			        + map->GetHeight(ex0, ey1) * (1-tx) *    ty
			        + map->GetHeight(ex1, ey1) *    tx  *    ty;
			h += HEIGHT_EPSILON;

			vertices.Add(FVector(mapX * worldScale, mapY * worldScale, h * worldScale));
			if (levelIdx <= 2)
				uvs.Add(FVector2D(vx / 32.f, vy / 32.f));
			else
				uvs.Add(FVector2D(mapX, mapY));
		}
	}

	// 让边缘在粗一级 LOD 的格点间距上保持分段线性，消除接缝裂缝
	auto vertIdx = [](int vx, int vy) { return vy * 33 + vx; };
	for (int edgeVy : { 0, 32 })
		for (int vx = 1; vx < 32; vx += 2)
			vertices[vertIdx(vx, edgeVy)] = (vertices[vertIdx(vx - 1, edgeVy)] + vertices[vertIdx(vx + 1, edgeVy)]) * 0.5f;
	for (int edgeVx : { 0, 32 })
		for (int vy = 1; vy < 32; vy += 2)
			vertices[vertIdx(edgeVx, vy)] = (vertices[vertIdx(edgeVx, vy - 1)] + vertices[vertIdx(edgeVx, vy + 1)]) * 0.5f;

	bool constructionRegion[4][4] = {};
	if (levelIdx <= 1) {
		int elemsPerRegion = 1 << levelIdx;
		int baseX = FMath::FloorToInt(startX);
		int baseY = FMath::FloorToInt(startY);
		for (int ri = 0; ri < 4; ri++)
			for (int rj = 0; rj < 4; rj++) {
				bool allConstruction = true;
				for (int dy = 0; dy < elemsPerRegion && allConstruction; dy++)
					for (int dx = 0; dx < elemsPerRegion && allConstruction; dx++) {
						int ex = FMath::Clamp(baseX + rj * elemsPerRegion + dx, 0, mapSize.first  - 1);
						int ey = FMath::Clamp(baseY + ri * elemsPerRegion + dy, 0, mapSize.second - 1);
						allConstruction = map->GetTerrain(ex, ey) == "construction";
					}
				constructionRegion[ri][rj] = allConstruction;
			}
	}

	for (int cy = 0; cy < 32; cy++) {
		for (int cx = 0; cx < 32; cx++) {
			int s = (cy / 8) * 4 + (cx / 8);
			if ((childPos >> (15 - s)) & 1) continue;
			if (constructionRegion[cy / 8][cx / 8]) continue;

			int32 v00 =  cy    * 33 + cx;
			int32 v10 =  cy    * 33 + cx + 1;
			int32 v01 = (cy+1) * 33 + cx;
			int32 v11 = (cy+1) * 33 + cx + 1;
			triangles.Add(v00); triangles.Add(v11); triangles.Add(v10);
			triangles.Add(v00); triangles.Add(v01); triangles.Add(v11);
		}
	}

	mesh->CreateMeshSection(0, vertices, triangles,
		TArray<FVector>(), uvs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	if (levelIdx <= 2) {
		// lod0/1/2 的 cell 比 element 还小，需要细分地形混合数据
		int32 baseRes = FMath::RoundToInt(size);
		TArray<FFineCell> cells;
		cells.SetNum(baseRes * baseRes);

		int baseX = FMath::FloorToInt(startX);
		int baseY = FMath::FloorToInt(startY);
		const auto& texInfo = map->GetTerrainTextures();
		for (int y = 0; y < baseRes; y++) {
			for (int x = 0; x < baseRes; x++) {
				int ex = FMath::Clamp(baseX + x, 0, mapSize.first  - 1);
				int ey = FMath::Clamp(baseY + y, 0, mapSize.second - 1);
				auto it = texInfo.find(map->GetTerrain(ex, ey));
				FFineCell& cell = cells[y * baseRes + x];
				cell.Ids[0] = (it != texInfo.end()) ? (uint8)it->second.first : 0;
				cell.Weights[0] = 1.f;
			}
		}

		int32 curRes = baseRes;
		int32 steps = 3 - levelIdx;
		for (int32 s = 0; s < steps; s++) {
			TArray<FFineCell> next;
			DownsampleFine(cells, curRes, next);
			cells = MoveTemp(next);
			curRes *= 2;
		}

		UTexture2D* fineIdx = UTexture2D::CreateTransient(32, 32, PF_B8G8R8A8);
		UTexture2D* finePow = UTexture2D::CreateTransient(32, 32, PF_B8G8R8A8);
		if (fineIdx && finePow) {
			fineIdx->Filter = TF_Nearest;
			fineIdx->SRGB = false;
			finePow->Filter = TF_Bilinear;
			finePow->SRGB = false;

			FTexturePlatformData* idxPd = fineIdx->GetPlatformData();
			FTexturePlatformData* powPd = finePow->GetPlatformData();
			if (idxPd && idxPd->Mips.Num() > 0 && powPd && powPd->Mips.Num() > 0) {
				TArray<FColor> idxPixels, powPixels;
				idxPixels.SetNum(32 * 32);
				powPixels.SetNum(32 * 32);
				for (int i = 0; i < 32 * 32; i++) {
					const FFineCell& cell = cells[i];
					idxPixels[i] = FColor(cell.Ids[0], cell.Ids[1], cell.Ids[2], cell.Ids[3]);
					powPixels[i] = FColor(
						(uint8)FMath::Clamp(FMath::RoundToInt(cell.Weights[0] * 255.f), 0, 255),
						(uint8)FMath::Clamp(FMath::RoundToInt(cell.Weights[1] * 255.f), 0, 255),
						(uint8)FMath::Clamp(FMath::RoundToInt(cell.Weights[2] * 255.f), 0, 255),
						(uint8)FMath::Clamp(FMath::RoundToInt(cell.Weights[3] * 255.f), 0, 255));
				}

				const int32 bytes = 32 * 32 * 4;
				void* idxData = idxPd->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(idxData, idxPixels.GetData(), bytes);
				idxPd->Mips[0].BulkData.Unlock();

				void* powData = powPd->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(powData, powPixels.GetData(), bytes);
				powPd->Mips[0].BulkData.Unlock();
			}
			fineIdx->UpdateResource();
			finePow->UpdateResource();
		}

		switch (levelIdx) {
		case 0: fineIndexMap0 = fineIdx; finePowerMap0 = finePow; break;
		case 1: fineIndexMap1 = fineIdx; finePowerMap1 = finePow; break;
		case 2: fineIndexMap2 = fineIdx; finePowerMap2 = finePow; break;
		default: break;
		}

		UMaterialInstanceDynamic* levelMat = nullptr;
		switch (levelIdx) {
		case 0: levelMat = fineMateria0; break;
		case 1: levelMat = fineMaterial1; break;
		case 2: levelMat = fineMaterial2; break;
		default: break;
		}
		if (levelMat) {
			levelMat->SetTextureParameterValue(TEXT("FineIndexMap"), fineIdx);
			levelMat->SetTextureParameterValue(TEXT("FinePowerMap"), finePow);
			levelMat->SetTextureParameterValue(TEXT("TerrainDiffuseArray"), terrainDiffuseArray);
			mesh->SetMaterial(0, levelMat);
		}
	}
	else if (terrainMaterial) {
		mesh->SetMaterial(0, terrainMaterial);
	}
}

void ATerrainBase::DownsampleFine(const TArray<FFineCell>& in, int32 inRes, TArray<FFineCell>& out) const {
	int32 outRes = inRes * 2;
	out.SetNum(outRes * outRes);

	for (int oy = 0; oy < outRes; oy++) {
		for (int ox = 0; ox < outRes; ox++) {
			float fx = (ox + 0.5f) * 0.5f - 0.5f;
			float fy = (oy + 0.5f) * 0.5f - 0.5f;
			int ix0 = FMath::FloorToInt(fx);
			int iy0 = FMath::FloorToInt(fy);
			float tx = fx - ix0;
			float ty = fy - iy0;
			int ix1 = ix0 + 1;
			int iy1 = iy0 + 1;
			ix0 = FMath::Clamp(ix0, 0, inRes - 1);
			ix1 = FMath::Clamp(ix1, 0, inRes - 1);
			iy0 = FMath::Clamp(iy0, 0, inRes - 1);
			iy1 = FMath::Clamp(iy1, 0, inRes - 1);

			const FFineCell* corners[4] = {
				&in[iy0 * inRes + ix0], &in[iy0 * inRes + ix1],
				&in[iy1 * inRes + ix0], &in[iy1 * inRes + ix1]
			};
			float cornerW[4] = { (1-tx)*(1-ty), tx*(1-ty), (1-tx)*ty, tx*ty };

			TMap<uint8, float> accum;
			for (int c = 0; c < 4; c++) {
				if (cornerW[c] <= 0.f) continue;
				for (int k = 0; k < 4; k++) {
					float w = corners[c]->Weights[k] * cornerW[c];
					if (w <= 0.f) continue;
					accum.FindOrAdd(corners[c]->Ids[k]) += w;
				}
			}

			TArray<TPair<uint8, float>> sorted;
			for (auto& pair : accum) sorted.Add(TPair<uint8, float>(pair.Key, pair.Value));
			sorted.Sort([](const TPair<uint8, float>& a, const TPair<uint8, float>& b) { return a.Value > b.Value; });

			FFineCell cell;
			for (int k = 0; k < 4; k++) {
				if (k < sorted.Num()) { cell.Ids[k] = sorted[k].Key; cell.Weights[k] = sorted[k].Value; }
			}
			out[oy * outRes + ox] = cell;
		}
	}
}

void ATerrainBase::BuildOceanMesh(int width, int height) {
	Map* map = global->GetMap();
	if (!map) return;

	const float worldScale = 1000.f;

	// 共享网格顶点：(width+1)*(height+1)个格点，高度固定为0
	TArray<FVector> vertices;
	TArray<FVector2D> uvs;
	vertices.Reserve((width + 1) * (height + 1));
	uvs.Reserve((width + 1) * (height + 1));
	for (int vy = 0; vy <= height; vy++)
		for (int vx = 0; vx <= width; vx++) {
			vertices.Add(FVector(vx * worldScale, vy * worldScale, 0.f));
			uvs.Add(FVector2D(vx, vy));
		}

	// 只为地形类型为"ocean"的格子生成两个三角形
	TArray<int32> triangles;
	auto vertIdx = [width](int vx, int vy) { return vy * (width + 1) + vx; };
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			auto [water, h] = map->GetWater(x, y);
			if (!water)continue;

			int idxBL = vertIdx(x, y), idxBR = vertIdx(x + 1, y);
			int idxTL = vertIdx(x, y + 1), idxTR = vertIdx(x + 1, y + 1);
			vertices[idxBL].Z = h * worldScale;
			vertices[idxBR].Z = h * worldScale;
			vertices[idxTL].Z = h * worldScale;
			vertices[idxTR].Z = h * worldScale;
			triangles.Add(idxBL); triangles.Add(idxTL); triangles.Add(idxTR);
			triangles.Add(idxBL); triangles.Add(idxTR); triangles.Add(idxBR);
		}
	}
	if (triangles.Num() == 0) return;

	if (!oceanMesh) {
		oceanMesh = NewObject<UProceduralMeshComponent>(this, TEXT("OceanMesh"));
		oceanMesh->SetupAttachment(RootComponent);
		oceanMesh->RegisterComponent();
		AddInstanceComponent(oceanMesh);
	}

	oceanMesh->CreateMeshSection(0, vertices, triangles,
		TArray<FVector>(), uvs, TArray<FColor>(), TArray<FProcMeshTangent>(), false);

	if (!oceanMaterial) {
		UMaterialInterface* baseMat = Cast<UMaterialInterface>(UAssetLoader::LoadAssetFromPath(
			TEXT("/Water/Materials/WaterSurface/Water_Material_CustomMesh.Water_Material_CustomMesh"), UMaterialInterface::StaticClass()));
		if (baseMat)
			oceanMaterial = UMaterialInstanceDynamic::Create(baseMat, this);
	}
	if (oceanMaterial)
		oceanMesh->SetMaterial(0, oceanMaterial);
}
