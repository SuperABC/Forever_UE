#include "Actor/PopulaceBase.h"
#include "Actor/GlobalBase.h"


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

	TArray<FPerson> citizens;
	auto populace = ((AGlobalBase*)global)->GetPopulace();
	for (auto citizen : populace->GetCitizens()) {
		if (personInstances.find(citizen->GetName()) != personInstances.end()) {
			continue;
		}
		FPerson citizenInfo;
		citizenInfo.name = UTF8_TO_TCHAR(citizen->GetName().data());
		if (!citizen->GetHome())continue;
		auto pos = citizen->GetHome()->GetParentBuilding()->GetPosition();
		citizenInfo.pos = FVector(pos.first, pos.second, 0.f);
		citizens.Add(citizenInfo);
	}
	UpdatePopulace(citizens);
}

void APopulaceBase::SetGlobal(AActor* g) {
	this->global = g;
}

void APopulaceBase::SetInstance(FString name, AActor* actor) {
	if (personInstances.find(TCHAR_TO_UTF8(*name)) == personInstances.end()) {
		personInstances[TCHAR_TO_UTF8(*name)] = actor;
	}
	else {
		THROW_EXCEPTION(InvalidConfigException, string("Duplicate person name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

