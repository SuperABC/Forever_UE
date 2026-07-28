#include "RoadnetBase.h"

#include "GlobalBase.h"

#include "map/map.h"
#include "map/roadnet.h"


using namespace std;

ARoadnetBase::ARoadnetBase() :
	dirty(true) {
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

void ARoadnetBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

void ARoadnetBase::MarkDirty() {
	dirty = true;
}

TArray<FRoad> ARoadnetBase::GetRoadnet() {
	Map* map = global->GetMap();
	if (!map)return {};

	TArray<FRoad> roads;
	for (auto road : map->GetRoadnet()->GetRoads()) {
		Node n1 = road->GetPoint(0.f);
		Node n2 = road->GetPoint(1.f);

		float t1x, t1y, t1z;
		road->GetTangent(0.f, t1x, t1y, t1z);
		float t2x, t2y, t2z;
		road->GetTangent(1.f, t2x, t2y, t2z);

		FRoad r;
		r.v1 = FVector(n1.GetX(), n1.GetY(), n1.GetZ());
		r.v2 = FVector(n2.GetX(), n2.GetY(), n2.GetZ());
		r.type = FString(road->GetType().data());
		r.tangent1 = FVector(t1x, t1y, t1z);
		r.tangent2 = FVector(t2x, t2y, t2z);
		for (auto& [node, weight] : road->GetControls()) {
			r.controls.Add(FVector(node.GetX(), node.GetY(), node.GetZ()));
			r.weights.Add(weight);
		}

		roads.Add(r);
	}

	return roads;
}

void ARoadnetBase::GetNavigations(TArray<FNode>& nodes, TArray<FConnection>& connections) {
	Map* map = global->GetMap();
	if (!map)return;

	auto navigationNodes = map->GetNavigationNodes();
	for (auto node : navigationNodes) {
		nodes.Add(FNode(FVector(node->GetX(), node->GetY(), node->GetZ()),FString(node->GetCategory().data())));
	}

	auto navigationConnections = map->GetNavigationConnections();
	for (auto connection : navigationConnections) {
		connections.Add(FConnection(FVector(connection->GetStart().GetX(), connection->GetStart().GetY(), connection->GetStart().GetZ()),
			FVector(connection->GetEnd().GetX(), connection->GetEnd().GetY(), connection->GetEnd().GetZ())));
	}
}
