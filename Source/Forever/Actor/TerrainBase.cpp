#include "Actor/TerrainBase.h"
#include "Actor/GlobalBase.h"


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
	TArray<FCoordinate> coordinates;
	for(auto j = FMath::Clamp(int(location.Y - 20), 0, size.second); j <= FMath::Clamp(int(location.Y + 20), 0, size.second); j++) {
		for (auto i = FMath::Clamp(int(location.X - 20), 0, size.first); i <= FMath::Clamp(int(location.X + 20), 0, size.first); i++) {
			if(terrainInstances[j][i] == -1) {
				coordinates.Add(FCoordinate(i, j));
			}
		}
	}
	UpdateTerrain(coordinates);
}

void ATerrainBase::InitInstances(int width, int height) {
	terrainInstances = vector<vector<int>>(height, vector<int>(width, -1));
}

void ATerrainBase::SetGlobal(AActor* g) {
	this->global = g;
}

void ATerrainBase::LookupTerrain(int x, int y, FString& type, float& height) {
	Map* map = ((AGlobalBase*)global)->GetMap();
	if (!map)return;

	type = FString(map->GetTerrain(x, y).data());
	height = map->GetElement(x, y)->GetHeight();
}

void ATerrainBase::SetInstance(int x, int y, int id) {
	auto size = ((AGlobalBase*)global)->GetMap()->GetSize();
	if (x < 0 || y < 0 || x >= size.first || y >= size.second) {
		return;
	}
	terrainInstances[y][x] = id;
}

