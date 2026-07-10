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

	void AddFront(Dialog* dialog, Script* ownerScript, const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);
	void AddBack(Dialog* dialog, Script* ownerScript, const std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	void MatchEvent(Event* e, Script* script,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	void ApplyChange(Change* change,
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

	/*
	* 应用调度节点产出的变化列表（内部负责释放传入的Change对象）
	* @changes: 调度节点产出的变化列表
	*/
	void ApplySchedulerChanges(std::vector<Change*>& changes);

protected:
	virtual void BeginPlay() override;

	// 检查所有计时器，对已到时的计时器触发time_up事件
	void CheckTimers();

	/*
	* 对一组Change逐一执行完整的系统级分发（地图/人口/剧情等），供MatchEvent和ApplySchedulerChanges复用
	* @changes: 待应用的变化列表
	* @getValues: 用于条件求值的变量查找链
	* @ownerScript: 触发这组Change的脚本（用于deactivate_milestone等场景），无则传nullptr
	*/
	void ApplyChanges(const std::vector<Change*>& changes,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues,
		Script* ownerScript = nullptr);

	/*
	* 按名称查找电梯轿厢（遍历所有建筑，包括园区内的建筑）
	* @name: 轿厢名称
	*/
	Cabin* FindCabin(const std::string& name);

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AGlobalBase* global;

	std::deque<Section> dialogQueue;
	bool interacting;
};
