#include "Actor/RoadnetBase.h"
#include "Actor/GlobalBase.h"


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

TArray<FConnection> ARoadnetBase::GetRoadnet() {
	Map* map = ((AGlobalBase*)global)->GetMap();
	if (!map)return {};

	TArray<FConnection> connections;
	for (auto connection : map->GetRoadnet()->GetConnections()) {
		Node n1 = connection.GetRoadnet()->GetNodes()[connection.GetV1()];
		Node n2 = connection.GetRoadnet()->GetNodes()[connection.GetV2()];
		connections.Add(FConnection(
			FVector(n1.GetX(), n1.GetY(), 0.f), FVector(n2.GetX(), n2.GetY(), 0.f), connection.GetWidth()));
	}

	return connections;
}