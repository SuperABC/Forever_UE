#include "TerrainBase.h"
#include "GlobalBase.h"

#include "map/terrain.h"


using namespace std;

ATerrainBase::ATerrainBase() {
	PrimaryActorTick.bCanEverTick = true;
}

ATerrainBase::~ATerrainBase() {

}

void ATerrainBase::BeginPlay() {
	Super::BeginPlay();
}

void ATerrainBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector location = FVector(0.f, 0.f, 0.f);
	((AGlobalBase*)global)->GetLocation(location);
	location /= 1000.f;

	auto size = ((AGlobalBase*)global)->GetMap()->GetSize();
	TArray<FCoordinate> adds;
	for(auto j = FMath::Clamp(int(location.Y - 10), 0, size.second); j <= FMath::Clamp(int(location.Y + 10), 0, size.second); j++) {
		for (auto i = FMath::Clamp(int(location.X - 10), 0, size.first); i <= FMath::Clamp(int(location.X + 10), 0, size.first); i++) {
			if(terrainInstances[j][i].Num() == 0) {
				adds.Add(FCoordinate(i, j));
			}
		}
	}

	TArray<FCoordinate> removes;
	for (int j = 0; j < terrainInstances.size(); j++) {
		for (int i = 0; i < terrainInstances[j].size(); i++) {
			if (terrainInstances[j][i].Num() > 0) {
				if (j < FMath::Clamp(int(location.Y - 10), 0, size.second) || j > FMath::Clamp(int(location.Y + 10), 0, size.second) ||
					i < FMath::Clamp(int(location.X - 10), 0, size.first) || i > FMath::Clamp(int(location.X + 10), 0, size.first)) {
					removes.Add(FCoordinate(i, j));
				}
			}
		}
	}

	UpdateTerrain(adds, removes);
}

void ATerrainBase::SetGlobal(AActor* g) {
	this->global = g;
}

void ATerrainBase::InitInstances(int width, int height) {
	terrainInstances = vector<vector<TArray<int>>>(height, vector<TArray<int>>(width));
}

void ATerrainBase::LookupTerrain(int x, int y, FString& type, float& height) {
	Map* map = ((AGlobalBase*)global)->GetMap();
	if (!map)return;

	type = FString(map->GetTerrain(x, y).data());
	height = map->GetHeight(x, y);
}

void ATerrainBase::SetInstance(int x, int y, TArray<int> ids) {
	auto size = ((AGlobalBase*)global)->GetMap()->GetSize();
	if (x < 0 || y < 0 || x >= size.first || y >= size.second) {
		return;
	}

	terrainInstances[y][x] = ids;
	for (auto id : ids) {
		idList.push_back({ x, y });
	}
}

void ATerrainBase::RemoveInstance(int x, int y, TArray<int>& ids) {
	auto size = ((AGlobalBase*)global)->GetMap()->GetSize();
	if (x < 0 || y < 0 || x >= size.first || y >= size.second) return;

	ids = terrainInstances[y][x];
	if (ids.Num() == 0) return;

	ids.Sort([](int a, int b) { return a > b; });

	for (int removedId : ids) {
		terrainInstances[y][x].Remove(removedId);

		idList.erase(idList.begin() + removedId);

		for (auto& row : terrainInstances) {
			for (auto& cellIds : row) {
				for (int& instanceId : cellIds) {
					if (instanceId > removedId) {
						instanceId--;
					}
				}
			}
		}
	}
}

