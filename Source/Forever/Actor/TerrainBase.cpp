#include "Actor/TerrainBase.h"
#include "TerrainBase.h"


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
		UpdateTerrain();
	}
}

void ATerrainBase::SetGlobal(AActor* g) {
	this->global = g;
}

void ATerrainBase::MarkDirty() {
	dirty = true;
}

void ATerrainBase::LookupTerrain(int x, int y, FString& type) {
	Map* map = ((AGlobalBase*)global)->GetMap();
	type = FString(map->GetTerrain(x, y).data());
}

