#include "StoryBase.h"

#include "GlobalBase.h"
#include "PopulaceBase.h"

#include "map/map.h"
#include "map/zone.h"
#include "map/building.h"
#include "populace/populace.h"
#include "populace/person.h"
#include "populace/scheduler.h"
#include "society/society.h"
#include "society/job.h"
#include "story/story.h"
#include "story/script.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "player/player.h"


using namespace std;

AStoryBase::AStoryBase() {
	PrimaryActorTick.bCanEverTick = true;
}

AStoryBase::~AStoryBase() {

}

void AStoryBase::BeginPlay() {
	Super::BeginPlay();
}

void AStoryBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (interacting == false && dialogQueue.size() > 0) {
		interacting = true;
		auto section = dialogQueue.front();
		if (section.IsBranch()) {
			TArray<FString> options;
			for (auto option : section.GetOptions()) {
				options.Add(UTF8_TO_TCHAR(option.GetOption().data()));
			}
			UpdateDialogBranch(options);
		}
		else {
			dialogQueue.pop_front();

			auto speaking = section.GetSpeaking();
			UpdateDialog(UTF8_TO_TCHAR(speaking.first.data()), UTF8_TO_TCHAR(speaking.second.data()));
		}
	}
}

void AStoryBase::SetGlobal(AActor* g) {
	this->global = g;
}

void AStoryBase::AddFront(Dialog* dialog) {
	for(int i = dialog->GetDialogs().size() - 1; i >= 0; i--) {
		dialogQueue.push_front(dialog->GetDialogs()[i]);
	}
}

void AStoryBase::AddBack(Dialog* dialog) {
	for (auto section : dialog->GetDialogs()) {
		dialogQueue.push_back(section);
	}
}

void AStoryBase::MatchEvent(Event* event, Script* script,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	if (!script) {
		return;
	}

	try {
		vector<Action> actions;
		auto pres = script->PreTrigger(event);
		actions.insert(actions.end(), pres.begin(), pres.end());
		auto matches = script->MatchEvent(event, getValues);
		actions.insert(actions.end(), matches.begin(), matches.end());
		auto posts = script->PostTrigger(event);
		actions.insert(actions.end(), posts.begin(), posts.end());

		for (auto action : actions) {
			visit([&](auto* ptr) {
				if constexpr (is_same_v<decltype(ptr), Dialog*>) {
					auto* dialog = dynamic_cast<Dialog*>(ptr);
					if (dialog->GetCondition().EvaluateBool(getValues)) {
						AddBack(dialog);
					}
				}
				else if constexpr (is_same_v<decltype(ptr), Change*>) {
					auto* change = dynamic_cast<Change*>(ptr);
					if (change->GetCondition().EvaluateBool(getValues)) {
						((AGlobalBase*)global)->GetMap()->ApplyChange(change, getValues);
						((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, getValues);
						((AGlobalBase*)global)->GetSociety()->ApplyChange(change, getValues);
						((AGlobalBase*)global)->GetStory()->ApplyChange(change, getValues);
						((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, getValues);
						((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, getValues);
						((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, getValues);

						ApplyChange(change, getValues);
					}
				}
				}, action);
		}
	}
	catch (ExceptionBase& e) {
		UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	}
}

void AStoryBase::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	auto type = change->GetType();

	if (type == "spawn_npc") {
		auto obj = dynamic_cast<SpawnNpcChange*>(change);

		Condition condition;
		condition.ParseCondition(obj->GetName());
		FString name = UTF8_TO_TCHAR(ToString(condition.EvaluateValue(getValues)).data());
		condition.ParseCondition(obj->GetAvatar());
		FString avatar = UTF8_TO_TCHAR(ToString(condition.EvaluateValue(getValues)).data());
		FVector location = FVector(0.f, 0.f, 0.f);
		((AGlobalBase*)global)->GetLocation(location);
		location /= 1000.f;
		location += FVector(1.f, 1.f, 0.f);
		((AGlobalBase*)global)->GetPopulaceActor()->SpawnNpc(name, avatar, location);
	}
}

void AStoryBase::FinishSection() {
	interacting = false;
}

bool AStoryBase::SelectOption(FString selected) {
	auto section = dialogQueue.front();
	auto options = section.GetOptions();

	auto story = ((AGlobalBase*)global)->GetStory();
	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};

	for(auto option : options) {
		if (UTF8_TO_TCHAR(option.GetOption().data()) == selected) {
			dialogQueue.pop_front();
			auto dialogs = option.GetDialogs();
			if (dialogs.size() > 0) {
				for (int i = dialogs.size() - 1; i >= 0; i--) {
					if (!dialogs[i]->GetCondition().EvaluateBool(getValues))continue;
					AddFront(dialogs[i]);
				}
			}
			auto changes = option.GetChanges();
			for (auto change : changes) {
				if (!change->GetCondition().EvaluateBool(getValues))continue;
				((AGlobalBase*)global)->GetMap()->ApplyChange(change, getValues);
				((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, getValues);
				((AGlobalBase*)global)->GetSociety()->ApplyChange(change, getValues);
				((AGlobalBase*)global)->GetStory()->ApplyChange(change, getValues);
				((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, getValues);
				((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, getValues);
				((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, getValues);

				ApplyChange(change, getValues);
			}
			interacting = false;
			return true;
		}
	}

	return false;
}

TArray<FString> AStoryBase::GetOptions(FString name) {
	TArray<FString> options;
	auto populace = ((AGlobalBase*)global)->GetPopulace();
	auto citizen = populace->GetCitizen(TCHAR_TO_UTF8(*name));
	if (!citizen) {
		THROW_EXCEPTION(InvalidArgumentException, string("Citizen not found: ") + TCHAR_TO_UTF8(*name) + ".\n");
	}
	for (auto option : citizen->GetOptions()) {
		options.Add(UTF8_TO_TCHAR(option.data()));
	}
	return options;
}

void AStoryBase::GameStart() {
	auto story = ((AGlobalBase*)global)->GetStory();
	auto event = new GameStartEvent();

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto zones = ((AGlobalBase*)global)->GetMap()->GetZones();
	for (auto [_, z] : zones) {
		getValues.push_back(
			[&](string name) -> pair<bool, ValueType> {
				return z->GetScript()->GetValue(name);
			});
		MatchEvent(event, z->GetScript(), getValues);
		getValues.pop_back();
		for (auto [__, b] : z->GetBuildings()) {
			getValues.push_back(
				[&](string name) -> pair<bool, ValueType> {
					return b->GetScript()->GetValue(name);
				});
			MatchEvent(event, b->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	auto buildings = ((AGlobalBase*)global)->GetMap()->GetBuildings();
	for (auto [_, b] : buildings) {
		getValues.push_back(
			[&](string name) -> pair<bool, ValueType> {
				return b->GetScript()->GetValue(name);
			});
		MatchEvent(event, b->GetScript(), getValues);
		getValues.pop_back();
	}

	auto citizens = ((AGlobalBase*)global)->GetPopulace()->GetCitizens();
	for (auto citizen : citizens) {
		getValues.push_back(
			[&](string name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](string name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	delete event;
}

void AStoryBase::ScriptMessage(FString message) {
	auto story = ((AGlobalBase*)global)->GetStory();
	auto event = new ScriptMessageEvent(TCHAR_TO_UTF8(*message));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto zones = ((AGlobalBase*)global)->GetMap()->GetZones();
	for (auto [_, z] : zones) {
		getValues.push_back(
			[&](string name) -> pair<bool, ValueType> {
				return z->GetScript()->GetValue(name);
			});
		MatchEvent(event, z->GetScript(), getValues);
		getValues.pop_back();
		for (auto [__, b] : z->GetBuildings()) {
			getValues.push_back(
				[&](string name) -> pair<bool, ValueType> {
					return b->GetScript()->GetValue(name);
				});
			MatchEvent(event, b->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	auto buildings = ((AGlobalBase*)global)->GetMap()->GetBuildings();
	for (auto [_, b] : buildings) {
		getValues.push_back(
			[&](string name) -> pair<bool, ValueType> {
				return b->GetScript()->GetValue(name);
			});
		MatchEvent(event, b->GetScript(), getValues);
		getValues.pop_back();
	}

	auto citizens = ((AGlobalBase*)global)->GetPopulace()->GetCitizens();
	for (auto citizen : citizens) {
		getValues.push_back(
			[&](string name) -> pair<bool, ValueType> {
				return citizen->GetScheduler()->GetScript()->GetValue(name);
			});
		MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
		getValues.pop_back();
		for (auto job : citizen->GetJobs()) {
			getValues.push_back(
				[&](string name) -> pair<bool, ValueType> {
					return job->GetScript()->GetValue(name);
				});
			MatchEvent(event, job->GetScript(), getValues);
			getValues.pop_back();
		}
	}

	delete event;
}

void AStoryBase::OptionDialog(FString name, FString option) {
	auto story = ((AGlobalBase*)global)->GetStory();
	auto event = new OptionDialogEvent(TCHAR_TO_UTF8(*name), TCHAR_TO_UTF8(*option));

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};
	MatchEvent(event, story->GetScript(), getValues);

	auto citizen = ((AGlobalBase*)global)->GetPopulace()->GetCitizen(TCHAR_TO_UTF8(*name));
	getValues.push_back(
		[&](string name) -> pair<bool, ValueType> {
			return citizen->GetScheduler()->GetScript()->GetValue(name);
		});
	MatchEvent(event, citizen->GetScheduler()->GetScript(), getValues);
	getValues.pop_back();
	for (auto job : citizen->GetJobs()) {
		getValues.push_back(
			[&](string name) -> pair<bool, ValueType> {
				return job->GetScript()->GetValue(name);
			});
		MatchEvent(event, job->GetScript(), getValues);
		getValues.pop_back();
	}

	delete event;
}


