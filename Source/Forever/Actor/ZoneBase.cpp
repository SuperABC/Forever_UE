#include "Actor/ZoneBase.h"


using namespace std;

AZoneBase::AZoneBase() {
	PrimaryActorTick.bCanEverTick = true;
}

AZoneBase::~AZoneBase() {

}

void AZoneBase::BeginPlay() {
	Super::BeginPlay();
}

void AZoneBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (dirty) {
		dirty = false;
		UpdateZone();
	}
}

void AZoneBase::SetGlobal(AActor* g) {
	this->global = g;
}

void AZoneBase::MarkDirty() {
	dirty = true;
}

