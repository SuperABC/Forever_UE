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

	if (dirty) {
		dirty = false;

		FVector location = FVector(0.f, 0.f, 0.f);
		((AGlobalBase*)global)->GetLocation(location);
		location /= 1000.f;

		auto size = ((AGlobalBase*)global)->GetMap()->GetSize();
		UpdateTerrain(FMath::Clamp(int(location.X - 10), 0, size.first),
			FMath::Clamp(int(location.Y - 10), 0, size.second),
			FMath::Clamp(int(location.X + 10), 0, size.first),
			FMath::Clamp(int(location.Y + 10), 0, size.second));
	}
}

void ATerrainBase::SetGlobal(AActor* g) {
	this->global = g;
}

void ATerrainBase::MarkDirty() {
	dirty = true;
}

void ATerrainBase::LookupTerrain(int x, int y, FString& type, float& height) {
	Map* map = ((AGlobalBase*)global)->GetMap();
	type = FString(map->GetTerrain(x, y).data());
	height = map->GetElement(x, y)->GetHeight();
}

