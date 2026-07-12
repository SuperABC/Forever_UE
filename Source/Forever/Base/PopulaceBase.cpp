#include "PopulaceBase.h"

#include "GlobalBase.h"
#include "StoryBase.h"

#include "map/map.h"
#include "map/building.h"
#include "map/room.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "populace/scheduler.h"
#include "populace/commute.h"
#include "society/job.h"
#include "story/story.h"
#include "story/event.h"
#include "story/script.h"


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

	static int step = 0;
	static int stride = 20;

	FVector location = FVector(0.f, 0.f, 0.f);
	global->GetLocation(location);
	location /= 1000.f;

	TArray<FPerson> adds;
	auto populace = global->GetPopulace();
	for (int j = step; j < populace->GetCitizens().size(); j += stride) {
		auto citizen = populace->GetCitizens()[j];
		if (personInstances.find(citizen->GetName()) != personInstances.end()) {
			continue;
		}
		auto destination = citizen->PopChange();
		if (!destination.empty()) {
			global->GetStoryActor()->NpcArrive(
				UTF8_TO_TCHAR(citizen->GetName().data()), UTF8_TO_TCHAR(destination.data()));
		}

		FPerson citizenInfo;
		citizenInfo.name = UTF8_TO_TCHAR(citizen->GetName().data());
		citizenInfo.avatar = UTF8_TO_TCHAR(citizen->GetAvatar().data());
		if (!citizen->GetCurrentRoom())continue;
		auto room = citizen->GetCurrentRoom();
		auto pos = room->GetPosition(room->GetSizeX() / 2.f, room->GetSizeY() / 2.f);
		pos.first += (GetRandom(11) / 10.f - 0.5f) * 0.4f;
		pos.second += (GetRandom(11) / 10.f - 0.5f) * 0.4f;
		citizenInfo.pos = FVector(pos.first, pos.second, room->GetLayer() * room->GetParentBuilding()->GetHeight());
		if ((location - citizenInfo.pos).Size() > 2.f || fabs(location.Z - citizenInfo.pos.Z) > 0.4f) {
			continue;
		}
		adds.Add(citizenInfo);
	}

	TArray<FString> removes;
	for(auto &[name, instance] : personInstances) {
		auto destination = populace->GetCitizen(name)->PopChange();
		if (!destination.empty()) {
			global->GetStoryActor()->NpcArrive(
				UTF8_TO_TCHAR(name.data()), UTF8_TO_TCHAR(destination.data()));
		}
		auto pos = instance->GetActorLocation() / 1000.f;
		if(!destination.empty() ||(pos - location).Size() > 4.f || fabs(pos.Z - location.Z) > 0.8f) {
			removes.Add(UTF8_TO_TCHAR(name.data()));
		}
	}
	UpdatePopulace(adds, removes);

	step = (step + 1) % stride;
}

void APopulaceBase::SetGlobal(AGlobalBase* g) {
	this->global = g;
}

AActor* APopulaceBase::GetInstance(FString name) {
	auto it = personInstances.find(TCHAR_TO_UTF8(*name));
	if (it == personInstances.end()) {
		return nullptr;
	}
	return it->second;
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

		auto person = global->GetPopulace()->GetCitizen(TCHAR_TO_UTF8(*name));
		if (person) {
			auto commute = person->GetCurrentCommute();
			if (commute) {
				commute->EndVisible();
			}
		}
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Person not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

