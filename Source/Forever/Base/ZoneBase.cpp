#include "ZoneBase.h"

#include "GlobalBase.h"
#include "StoryBase.h"

#include "map/map.h"
#include "map/block.h"
#include "map/roadnet.h"
#include "map/zone.h"
#include "story/story.h"
#include "story/script.h"


using namespace std;

AZoneBase::AZoneBase() {
	PrimaryActorTick.bCanEverTick = true;
}

AZoneBase::~AZoneBase() {

}

void AZoneBase::BeginPlay() {
	Super::BeginPlay();
}

void AZoneBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector location = FVector(0.f, 0.f, 0.f);
	((AGlobalBase*)global)->GetLocation(location);
	location /= 1000.f;

	TArray<FZone> zones;
	auto blocks = ((AGlobalBase*)global)->GetMap()->GetRoadnet()->GetBlocks();
	for(auto block : blocks) {
		FVector blockLocation = FVector(block->GetPosX(), block->GetPosY(), 0.f);
		if((location - blockLocation).Size() > 64.f) {
			continue;
		}
		auto blockZones = block->GetZones();
		for (auto blockZone : blockZones) {
			if(zoneInstances.find(blockZone.first) != zoneInstances.end()) {
				continue;
			}
			auto zone = blockZone.second;
			FZone zoneInfo;
			zoneInfo.name = UTF8_TO_TCHAR(blockZone.first.data());
			float x, y;
			zone->GetPosition(x, y);
			zoneInfo.center = FVector(x, y, 0.f);
			zoneInfo.size = FVector(zone->GetSizeX(), zone->GetSizeY(), 100.f);
			zoneInfo.rotation = block->GetRotation();
			zoneInfo.fences.Add(
				FFence(FVector(zone->GetSizeX() / 2.f - 0.2f, 0.f, 0.f),
					FVector(0.1f, zone->GetSizeY() - 0.4f, 0.1f)));
			zoneInfo.fences.Add(
				FFence(FVector(-zone->GetSizeX() / 2.f + 0.2f, 0.f, 0.f),
					FVector(0.1f, zone->GetSizeY() - 0.4f, 0.1f)));
			zoneInfo.fences.Add(
				FFence(FVector(0.f, zone->GetSizeY() / 2.f - 0.2f, 0.f),
					FVector(zone->GetSizeX() - 0.4f, 0.1f, 0.1f)));
			zoneInfo.fences.Add(
				FFence(FVector(0.f, -zone->GetSizeY() / 2.f + 0.2f, 0.f),
					FVector(zone->GetSizeX() - 0.4f, 0.1f, 0.1f)));
			zones.Add(zoneInfo);
		}
	}
	UpdateZone(zones);
}

void AZoneBase::SetGlobal(AActor* g) {
	this->global = g;
}

void AZoneBase::SetInstance(FString name, AActor* actor) {
	if (zoneInstances.find(TCHAR_TO_UTF8(*name)) == zoneInstances.end()) {
		zoneInstances[TCHAR_TO_UTF8(*name)] = actor;
	}
	else {
		THROW_EXCEPTION(RuntimeException, string("Duplicate zone name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

void AZoneBase::EnterZone(FString zone) {
	auto storyBase = ((AGlobalBase*)global)->GetStoryActor();
	auto story = ((AGlobalBase*)global)->GetStory();
	auto event = new EnterZoneEvent(TCHAR_TO_UTF8(*zone));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	storyBase->MatchEvent(event, story->GetScript(), getValues);

	auto z = ((AGlobalBase*)global)->GetMap()->GetZone(TCHAR_TO_UTF8(*zone));
	getValues.push_back(
		[&](string name) -> pair<bool, ValueType> {
			return z->GetScript()->GetValue(name);
		});
	storyBase->MatchEvent(event, z->GetScript(), getValues);
	getValues.pop_back();
	
	delete event;
}

void AZoneBase::LeaveZone(FString zone) {
	auto storyBase = ((AGlobalBase*)global)->GetStoryActor();
	auto story = ((AGlobalBase*)global)->GetStory();
	auto event = new LeaveZoneEvent(TCHAR_TO_UTF8(*zone));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	storyBase->MatchEvent(event, story->GetScript(), getValues);

	auto zones = ((AGlobalBase*)global)->GetMap()->GetZones();
	for (auto [_, z] : zones) {
		getValues.push_back(
			[&](string name) -> pair<bool, ValueType> {
				return z->GetScript()->GetValue(name);
			});
		storyBase->MatchEvent(event, z->GetScript(), getValues);
		getValues.pop_back();
	}

	delete event;
}
