#include "Actor/StoryBase.h"
#include "Actor/GlobalBase.h"


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

void AStoryBase::ApplyChange(Change* change,
	vector<function<pair<bool, ValueType>(const string&)>>& getValues) {
	auto type = change->GetType();

	//if (type == "spawn_npc") {
	//	auto obj = dynamic_cast<SpawnNpcChange*>(change);

	//	Condition condition;
	//	condition.ParseCondition(obj->GetTarget());
	//	FString name = UTF8_TO_TCHAR(ToString(condition.EvaluateValue(getValues)).data());
	//	condition.ParseCondition(obj->GetAvatar());
	//	FString avatar = UTF8_TO_TCHAR(ToString(condition.EvaluateValue(getValues)).data());
	//	FVector location = FVector(0.f, 0.f, 0.f);
	//	((AGlobalBase*)global)->GetLocation(location);
	//	location /= 1000.f;
	//	location += FVector(1.f, 1.f, 0.f);
	//	((AGlobalBase*)global)->GetPopulaceActor()->SpawnNpc(name, avatar, location);
	//}
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
				((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
				((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

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
	//auto populace = ((AGlobalBase*)global)->GetPopulace();
	//auto citizen = populace->GetCitizen(TCHAR_TO_UTF8(*name));
	//for (auto option : citizen->GetOptions()) {
	//	options.Add(UTF8_TO_TCHAR(option.data()));
	//}
	return options;
}

void AStoryBase::GameStart() {
	auto story = ((AGlobalBase*)global)->GetStory();
	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetScript()->GetValue(name);
		}
	};

	auto event = new GameStartEvent();
	try {
		std::vector<Action> actions;
		auto pres = story->GetScript()->PreTrigger(event);
		actions.insert(actions.end(), pres.begin(), pres.end());
		auto matches = story->GetScript()->MatchEvent(event, getValues);
		actions.insert(actions.end(), matches.begin(), matches.end());
		auto posts = story->GetScript()->PostTrigger(event);
		actions.insert(actions.end(), posts.begin(), posts.end());

		for (auto action : actions) {
			std::visit([&](auto* ptr) {
				if constexpr (std::is_same_v<decltype(ptr), Dialog*>) {
					auto* dialog = dynamic_cast<Dialog*>(ptr);
					if (dialog->GetCondition().EvaluateBool(getValues)) {
						AddBack(dialog);
					}
				}
				else if constexpr (std::is_same_v<decltype(ptr), Change*>) {
					auto* change = dynamic_cast<Change*>(ptr);
					if (change->GetCondition().EvaluateBool(getValues)) {
						((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
						((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
						((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
						((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
						((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
						((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
						((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

						ApplyChange(change, getValues);
					}
				}
				}, action);
		}
	} catch (ExceptionBase& e) {
		UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	}

	delete event;
}

void AStoryBase::OptionDialog(FString name, FString option) {
	//auto story = ((AGlobalBase*)global)->GetStory();
	//auto event = new OptionDialogEvent(TCHAR_TO_UTF8(*name), TCHAR_TO_UTF8(*option));

	//try {
	//	auto [dialogs, changes] = story->MatchEvent(event);
	//	for (auto& dialog : dialogs) {
	//		if (story->JudgeCondition(dialog.GetCondition())) {
	//			AddBack(&dialog);
	//		}
	//	}
	//	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
	//		[&](string name) -> pair<bool, ValueType> {
	//			return story->GetValue(name);
	//		}
	//	};
	//	for (auto change : changes) {
	//		if (!story->JudgeCondition(change->GetCondition()))continue;
	//		((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

	//		ApplyChange(change, getValues);
	//	}

	//	((AGlobalBase*)global)->GetPopulaceActor()->TriggerEvent(event);
	//} catch (ExceptionBase& e) {
	//	UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	//}

	//delete event;
}

void AStoryBase::EnterZone(FString zone) {
	//auto story = ((AGlobalBase*)global)->GetStory();
	//auto event = new EnterZoneEvent(TCHAR_TO_UTF8(*zone));

	//try {
	//	auto [dialogs, changes] = story->MatchEvent(event);
	//	for (auto& dialog : dialogs) {
	//		if (story->JudgeCondition(dialog.GetCondition())) {
	//			AddBack(&dialog);
	//		}
	//	}
	//	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
	//		[&](string name) -> pair<bool, ValueType> {
	//			return story->GetValue(name);
	//		}
	//	};
	//	for (auto change : changes) {
	//		if (!story->JudgeCondition(change->GetCondition()))continue;
	//		((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

	//		ApplyChange(change, getValues);
	//	}

	//	((AGlobalBase*)global)->GetPopulaceActor()->TriggerEvent(event);
	//}
	//catch (ExceptionBase& e) {
	//	UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	//}

	//delete event;
}

void AStoryBase::LeaveZone(FString zone) {
	//auto story = ((AGlobalBase*)global)->GetStory();
	//auto event = new LeaveZoneEvent(TCHAR_TO_UTF8(*zone));

	//try {
	//	auto [dialogs, changes] = story->MatchEvent(event);
	//	for (auto& dialog : dialogs) {
	//		if (story->JudgeCondition(dialog.GetCondition())) {
	//			AddBack(&dialog);
	//		}
	//	}
	//	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
	//		[&](string name) -> pair<bool, ValueType> {
	//			return story->GetValue(name);
	//		}
	//	};
	//	for (auto change : changes) {
	//		if (!story->JudgeCondition(change->GetCondition()))continue;
	//		((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

	//		ApplyChange(change, getValues);
	//	}

	//	((AGlobalBase*)global)->GetPopulaceActor()->TriggerEvent(event);
	//}
	//catch (ExceptionBase& e) {
	//	UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	//}

	//delete event;
}

void AStoryBase::EnterBuilding(FString zone, FString building) {
	//auto story = ((AGlobalBase*)global)->GetStory();
	//auto event = new EnterBuildingEvent(TCHAR_TO_UTF8(*zone), TCHAR_TO_UTF8(*building));

	//try {
	//	auto [dialogs, changes] = story->MatchEvent(event);
	//	for (auto& dialog : dialogs) {
	//		if (story->JudgeCondition(dialog.GetCondition())) {
	//			AddBack(&dialog);
	//		}
	//	}
	//	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
	//		[&](string name) -> pair<bool, ValueType> {
	//			return story->GetValue(name);
	//		}
	//	};
	//	for (auto change : changes) {
	//		if (!story->JudgeCondition(change->GetCondition()))continue;
	//		((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

	//		ApplyChange(change, getValues);
	//	}

	//	((AGlobalBase*)global)->GetPopulaceActor()->TriggerEvent(event);
	//}
	//catch (ExceptionBase& e) {
	//	UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	//}

	//delete event;
}

void AStoryBase::LeaveBuilding(FString zone, FString building) {
	//auto story = ((AGlobalBase*)global)->GetStory();
	//auto event = new LeaveBuildingEvent(TCHAR_TO_UTF8(*zone), TCHAR_TO_UTF8(*building));

	//try {
	//	auto [dialogs, changes] = story->MatchEvent(event);
	//	for (auto& dialog : dialogs) {
	//		if (story->JudgeCondition(dialog.GetCondition())) {
	//			AddBack(&dialog);
	//		}
	//	}
	//	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
	//		[&](string name) -> pair<bool, ValueType> {
	//			return story->GetValue(name);
	//		}
	//	};
	//	for (auto change : changes) {
	//		if (!story->JudgeCondition(change->GetCondition()))continue;
	//		((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

	//		ApplyChange(change, getValues);
	//	}

	//	((AGlobalBase*)global)->GetPopulaceActor()->TriggerEvent(event);
	//}
	//catch (ExceptionBase& e) {
	//	UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	//}

	//delete event;
}

void AStoryBase::EnterRoom(FString zone, FString building, FString room) {
	//auto story = ((AGlobalBase*)global)->GetStory();
	//auto event = new EnterRoomEvent(
	//	TCHAR_TO_UTF8(*zone), TCHAR_TO_UTF8(*building), TCHAR_TO_UTF8(*room));

	//try {
	//	auto [dialogs, changes] = story->MatchEvent(event);
	//	for (auto& dialog : dialogs) {
	//		if (story->JudgeCondition(dialog.GetCondition())) {
	//			AddBack(&dialog);
	//		}
	//	}
	//	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
	//		[&](string name) -> pair<bool, ValueType> {
	//			return story->GetValue(name);
	//		}
	//	};
	//	for (auto change : changes) {
	//		if (!story->JudgeCondition(change->GetCondition()))continue;
	//		((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

	//		ApplyChange(change, getValues);
	//	}

	//	((AGlobalBase*)global)->GetPopulaceActor()->TriggerEvent(event);
	//}
	//catch (ExceptionBase& e) {
	//	UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	//}

	//delete event;
}

void AStoryBase::LeaveRoom(FString zone, FString building, FString room) {
	//auto story = ((AGlobalBase*)global)->GetStory();
	//auto event = new LeaveRoomEvent(
	//	TCHAR_TO_UTF8(*zone), TCHAR_TO_UTF8(*building), TCHAR_TO_UTF8(*room));

	//try {
	//	auto [dialogs, changes] = story->MatchEvent(event);
	//	for (auto& dialog : dialogs) {
	//		if (story->JudgeCondition(dialog.GetCondition())) {
	//			AddBack(&dialog);
	//		}
	//	}
	//	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
	//		[&](string name) -> pair<bool, ValueType> {
	//			return story->GetValue(name);
	//		}
	//	};
	//	for (auto change : changes) {
	//		if (!story->JudgeCondition(change->GetCondition()))continue;
	//		((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
	//		((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);

	//		ApplyChange(change, getValues);
	//	}

	//	((AGlobalBase*)global)->GetPopulaceActor()->TriggerEvent(event);
	//}
	//catch (ExceptionBase& e) {
	//	UE_LOGFMT(LogTemp, Log, "Exception: {0}", FString(UTF8_TO_TCHAR(e.GetDetailedInfo().data())));
	//}

	//delete event;
}


