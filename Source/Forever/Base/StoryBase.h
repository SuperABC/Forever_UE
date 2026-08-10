#pragma once

#define NOMINMAX

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GlobalBase.h"

#include "common/class.h"
#include "story/dialog.h"

#include <deque>

#include "StoryBase.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct FItem {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Industry")
	FString name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Industry")
	float price;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Industry")
	float amount;
};

UCLASS()
class FOREVER_API AStoryBase : public AActor {
	GENERATED_BODY()
	
public:	
	AStoryBase();
	~AStoryBase();

	virtual void Tick(float DeltaTime) override;

	void SetGlobal(AGlobalBase* g);

	void AddFront(const Dialog* dialog, Script* ownerScript, const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);
	void AddBack(const Dialog* dialog, Script* ownerScript, const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	void MatchEvent(Event* e, Script* script,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	std::vector<Event*> ApplyChange(const Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		Script* ownerScript = nullptr);

	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void UpdateDialog(const FString& speaker, const FString& content, const FString& label, const FString& voice);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void UpdateDialogBranch(const TArray<FString>& options);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void RemoveOption(const FString& name, const FString& option);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void OpenShop(const TArray<FItem>& items);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void StartPuzzle(const FString& puzzle);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void EnterBattle(AStoryBase* story, const FString& enemy);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void PlayVideo(const FString& path);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void PlayBgm(const FString& bgm, bool loop);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void StopBgm();
	UFUNCTION(BlueprintCallable, Category = "Story")
	void FinishSection();
	UFUNCTION(BlueprintCallable, Category = "Story")
	bool SelectOption(FString selected);
	UFUNCTION(BlueprintCallable, Category = "Story")
	TArray<FString> GetOptions(FString name);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void GetTask(FString& task);
	UFUNCTION(BlueprintCallable, Category = "Story")
	FString GetHistory();
	UFUNCTION(BlueprintCallable, Category = "Story")
	void SetStatus(const FString& name, const FString& destination);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void GameStart();
	UFUNCTION(BlueprintCallable, Category = "Story")
	void ScriptMessage(FString message);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void OptionDialog(FString name, FString option);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void SpeakingFinish(FString label);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void BattleWin(FString enemy);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void BattleLose(FString enemy);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void PuzzleResult(int result);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void NpcArrive(const FString& name, const FString& destination);

	void ApplyChanges(const std::vector<const Change*>& changes,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		Script* ownerScript = nullptr);
	void ApplyChanges(const std::vector<std::pair<Change*, Script*>>& changes,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

protected:
	virtual void BeginPlay() override;

	void ProcessExpiredTimers(int maxCount);
	void SimulateDayBoundary(const Time& moment);

	Cabin* FindCabin(const std::string& name);

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

	std::deque<Section> dialogQueue;
	bool interacting;
};
