#include "PopulaceBase.h"

#include "GlobalBase.h"

#include "map/building.h"
#include "map/room.h"
#include "populace/populace.h"
#include "populace/person.h"


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

	FVector location = FVector(0.f, 0.f, 0.f);
	((AGlobalBase*)global)->GetLocation(location);
	location /= 1000.f;

	TArray<FPerson> adds;
	auto populace = ((AGlobalBase*)global)->GetPopulace();
	for (auto citizen : populace->GetCitizens()) {
		if (personInstances.find(citizen->GetName()) != personInstances.end()) {
			continue;
		}
		citizen->PopChange();
		FPerson citizenInfo;
		citizenInfo.name = UTF8_TO_TCHAR(citizen->GetName().data());
		if (!citizen->GetCurrentRoom())continue;
		auto room = citizen->GetCurrentRoom();
		auto pos = room->GetPosition(room->GetSizeX() / 2.f, room->GetSizeY() / 2.f);
		citizenInfo.pos = FVector(pos.first, pos.second, room->GetLayer() * room->GetParentBuilding()->GetHeight());
		if ((location - citizenInfo.pos).Size() > 8.f) {
			continue;
		}
		adds.Add(citizenInfo);
	}

	TArray<FString> removes;
	for(auto &[name, instance] : personInstances) {
		auto change = populace->GetCitizen(name)->PopChange();
		if(change || (instance->GetActorLocation() / 1000.f - location).Size() > 16.f) {
			removes.Add(UTF8_TO_TCHAR(name.data()));
		}
	}
	UpdatePopulace(adds, removes);
}

void APopulaceBase::SetGlobal(AActor* g) {
	this->global = g;
}

void APopulaceBase::SpawnNpc(const FString& name, const FString& avatar, const FVector& position) {
	UpdatePopulace({ FPerson(name, avatar, position) }, { });
}

void APopulaceBase::AddInstance(FString name, AActor* actor) {
	if (personInstances.find(TCHAR_TO_UTF8(*name)) == personInstances.end()) {
		personInstances[TCHAR_TO_UTF8(*name)] = actor;
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Duplicate person name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

void APopulaceBase::RemoveInstance(FString name, AActor*& instance) {
	if (personInstances.find(TCHAR_TO_UTF8(*name)) != personInstances.end()) {
		instance = personInstances[TCHAR_TO_UTF8(*name)];
		personInstances.erase(TCHAR_TO_UTF8(*name));
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Person not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

