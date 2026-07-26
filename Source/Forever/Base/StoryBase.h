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
	void UpdateDialog(const FString& speaker, const FString& content, const FString& label);
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

	/*
	* 检查并执行到期的全局计时器
	* @maxCount: 本次最多执行的计时器数量，正常帧率节流时传MAX_TIMERS_PER_CHECK，跨天时间跳变时传UNLIMITED_TIMERS
	*/
	void ProcessExpiredTimers(int maxCount);

	/*
	* 将游戏时钟瞬间推进到指定时刻，并手动模拟一次Tick（刷新剧情时间变量、职业/组织/市民日程、生产结算、全局计时器），用于change_time等跨天时间跳变
	* @moment: 目标时刻
	*/
	void SimulateDayBoundary(const Time& moment);

	Cabin* FindCabin(const std::string& name);

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

	std::deque<Section> dialogQueue;
	bool interacting;
};
