#include "Actor/PopulaceBase.h"


using namespace std;

APopulaceBase::APopulaceBase() {
	PrimaryActorTick.bCanEverTick = true;
}

APopulaceBase::~APopulaceBase() {

}

void APopulaceBase::BeginPlay() {
	Super::BeginPlay();
}

void APopulaceBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (dirty) {
		dirty = false;
		UpdatePopulace();
	}
}

void APopulaceBase::SetGlobal(AActor* g) {
	this->global = g;
}

void APopulaceBase::MarkDirty() {
	dirty = true;
}

