#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#define NOMINMAX

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

	void SetGlobal(AActor* g);

	void AddFront(Dialog* dialog);
	void AddBack(Dialog* dialog);

	void MatchEvent(Event* e, Script* script,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void UpdateDialog(const FString& speaker, const FString& content, const FString& label);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void UpdateDialogBranch(const TArray<FString>& options);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void OpenShop(const TArray<FItem>& items);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void EnterBattle(AStoryBase* story, const FString& enemy);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void FinishSection();
	UFUNCTION(BlueprintCallable, Category = "Story")
	bool SelectOption(FString selected);
	UFUNCTION(BlueprintCallable, Category = "Story")
	TArray<FString> GetOptions(FString name);

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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AActor* global;

	std::deque<Section> dialogQueue;
	bool interacting = false;
};
