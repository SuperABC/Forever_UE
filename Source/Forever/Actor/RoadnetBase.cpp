#include "Actor/RoadnetBase.h"


using namespace std;

ARoadnetBase::ARoadnetBase() {
	PrimaryActorTick.bCanEverTick = true;
}

ARoadnetBase::~ARoadnetBase() {

}

void ARoadnetBase::BeginPlay() {
	Super::BeginPlay();
}

void ARoadnetBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (dirty) {
		dirty = false;
		UpdateRoadnet();
	}
}

void ARoadnetBase::SetGlobal(AActor* g) {
	this->global = g;
}

void ARoadnetBase::MarkDirty() {
	dirty = true;
}
