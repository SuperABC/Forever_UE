#include "Actor/TrafficBase.h"


using namespace std;

ATrafficBase::ATrafficBase() {
	PrimaryActorTick.bCanEverTick = true;
}

ATrafficBase::~ATrafficBase() {

}

void ATrafficBase::BeginPlay() {
	Super::BeginPlay();
}

void ATrafficBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (dirty) {
		dirty = false;
		UpdateTraffic();
	}
}

void ATrafficBase::SetGlobal(AActor* g) {
	this->global = g;
}

void ATrafficBase::MarkDirty() {
	dirty = true;
}

