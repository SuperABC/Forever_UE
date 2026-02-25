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

	TArray<FPerson> adds;
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
		if ((location - citizenInfo.pos).Size() > 8.f) {
			continue;
		}
		adds.Add(citizenInfo);
	}

	TMap<FString, AActor*> removes;
	for(auto &[name, instance] : personInstances) {
		if((instance->GetActorLocation() / 1000.f - location).Size() > 16.f) {
			removes.Add(UTF8_TO_TCHAR(name.data()), instance);
		}
	}
	UpdatePopulace(adds, removes);
}

void APopulaceBase::SetGlobal(AActor* g) {
	this->global = g;
}

void APopulaceBase::TriggerEvent(Event* event) {
	auto story = ((AGlobalBase*)global)->GetStory();
	auto populace = ((AGlobalBase*)global)->GetPopulace();

	bool traverse = false;
	int id = -1;
	Person* person = nullptr;
	auto type = event->GetType();
	if (type == "game_start") {
		traverse = true;
	}
	else if(type == "option_dialog") {
		auto target = dynamic_cast<OptionDialogEvent*>(event)->GetTarget();
		auto idx = dynamic_cast<OptionDialogEvent*>(event)->GetIdx();
		if (target.size() == 0) {
			if (idx >= populace->GetCitizens().size() || idx < 0) {
				THROW_EXCEPTION(CommandException, "Wrong input citizen ID.");
			}
			person = populace->GetCitizens()[idx];
			id = idx;
		}
		else {
			person = populace->GetCitizen(target);
			if (person == nullptr) {
				THROW_EXCEPTION(CommandException, "Citizen not found.");
			}
			id = person->GetId();
		}
	}

	if (traverse) {
		for (int i = 0; i < populace->GetCitizens().size(); i++) {
			vector<function<pair<bool, ValueType>(const string&)>> getValues = {
				[&](string name) -> pair<bool, ValueType> {
					return story->GetValue(name);
				},
				[&](string name) -> pair<bool, ValueType> {
					return populace->GetCitizens()[i]->GetValue(name);
				}
			};
			auto [dialogs, changes] = populace->TriggerEvent(i, event, story);
			for (auto dialog : dialogs) {
				if (story->JudgeCondition(dialog.GetCondition())) {
					((AGlobalBase*)global)->GetStoryActor()->AddBack(&dialog);
				}
			}
			for (auto change : changes) {
				if (!story->JudgeCondition(change->GetCondition()))continue;
				((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

				((AGlobalBase*)global)->GetStoryActor()->ApplyChange(change, getValues);
			}
		}
	}
	else {
		vector<function<pair<bool, ValueType>(const string&)>> getValues = {
			[&](string name) -> pair<bool, ValueType> {
				return story->GetValue(name);
			},
			[&](string name) -> pair<bool, ValueType> {
				return person->GetValue(name);
			}
		};
		auto [dialogs, changes] = populace->TriggerEvent(id, event, story);
		for (auto dialog : dialogs) {
			if (story->JudgeCondition(dialog.GetCondition())) {
				((AGlobalBase*)global)->GetStoryActor()->AddBack(&dialog);
			}
		}
		for (auto change : changes) {
			if (!story->JudgeCondition(change->GetCondition()))continue;
			((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
			((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
			((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
			((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
			((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
			((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
			((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

			((AGlobalBase*)global)->GetStoryActor()->ApplyChange(change, getValues);
		}
	}
}

void APopulaceBase::SpawnNpc(const FString& name, const FString& avatar, const FVector& position) {
	UpdatePopulace({ FPerson(name, avatar, position) }, { });
}

void APopulaceBase::AddInstance(FString name, AActor* actor) {
	if (personInstances.find(TCHAR_TO_UTF8(*name)) == personInstances.end()) {
		personInstances[TCHAR_TO_UTF8(*name)] = actor;
	}
	else {
		THROW_EXCEPTION(InvalidConfigException, string("Duplicate person name: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

void APopulaceBase::RemoveInstance(FString name) {
	if (personInstances.find(TCHAR_TO_UTF8(*name)) != personInstances.end()) {
		personInstances.erase(TCHAR_TO_UTF8(*name));
	}
	else {
		THROW_EXCEPTION(InvalidConfigException, string("Person not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
}

TArray<FString> APopulaceBase::GetOptions(FString name) {
	TArray<FString> options;
	auto populace = ((AGlobalBase*)global)->GetPopulace();
	auto citizen = populace->GetCitizen(TCHAR_TO_UTF8(*name));
	if (!citizen) {
		THROW_EXCEPTION(InvalidArgumentException, string("Citizen not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
	for(auto option : citizen->GetOptions()) {
		options.Add(UTF8_TO_TCHAR(option.data()));
	}
	return options;
}

