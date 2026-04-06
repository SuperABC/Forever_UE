#include "TrafficBase.h"


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

}

void ATrafficBase::SetGlobal(AActor* g) {
	this->global = g;
}


