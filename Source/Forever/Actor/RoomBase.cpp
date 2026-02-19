#include "Actor/RoomBase.h"


using namespace std;

ARoomBase::ARoomBase() {
	PrimaryActorTick.bCanEverTick = true;
}

ARoomBase::~ARoomBase() {

}

void ARoomBase::BeginPlay() {
	Super::BeginPlay();
}

void ARoomBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	UpdateRoom();
}

void ARoomBase::SetGlobal(AActor* g) {
	this->global = g;
}


