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

void AStoryBase::FinishSection() {
	interacting = false;
}

bool AStoryBase::SelectOption(FString selected) {
	auto section = dialogQueue.front();
	auto options = section.GetOptions();
	
	for(auto option : options) {
		if (UTF8_TO_TCHAR(option.GetOption().data()) == selected) {
			dialogQueue.pop_front();
			auto story = ((AGlobalBase*)global)->GetStory();
			auto dialogs = option.GetDialogs();
			if (dialogs.size() > 0) {
				for (int i = dialogs.size() - 1; i >= 0; i--) {
					if (!story->JudgeCondition(dialogs[i].GetCondition()))continue;
					AddFront(&dialogs[i]);
				}
			}
			interacting = false;
			return true;
		}
	}

	return false;
}

void AStoryBase::GameStart() {
	auto story = ((AGlobalBase*)global)->GetStory();
	auto event = new GameStartEvent();

	auto [dialogs, changes] = story->MatchEvent(event);

	vector<function<pair<bool, ValueType>(const string&)>> getValues = {
		[&](string name) -> pair<bool, ValueType> {
			return story->GetValue(name);
		}
	};
	for (auto change : changes) {
		if (!story->JudgeCondition(change->GetCondition()))continue;
		((AGlobalBase*)global)->GetMap()->ApplyChange(change, story, getValues);
		((AGlobalBase*)global)->GetPopulace()->ApplyChange(change, story, getValues);
		((AGlobalBase*)global)->GetSociety()->ApplyChange(change, story, getValues);
		((AGlobalBase*)global)->GetStory()->ApplyChange(change, story, getValues);
		((AGlobalBase*)global)->GetIndustry()->ApplyChange(change, story, getValues);
		((AGlobalBase*)global)->GetTraffic()->ApplyChange(change, story, getValues);
		((AGlobalBase*)global)->GetPlayer()->ApplyChange(change, story, getValues);
	}

	for (auto& dialog : dialogs) {
		if (story->JudgeCondition(dialog.GetCondition())) {
			AddBack(&dialog);
		}
	}
}

