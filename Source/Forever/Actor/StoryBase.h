#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "story/story.h"

#include <deque>

#include "StoryBase.generated.h"


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

	void ApplyChange(Change* change,
		std::vector<std::function<std::pair<bool, ValueType>(const std::string&)>>& getValues);

	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void UpdateDialog(const FString& speaker, const FString& content);
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void UpdateDialogBranch(const TArray<FString>& options);
	UFUNCTION(BlueprintCallable, Category = "Story")
	void FinishSection();
	UFUNCTION(BlueprintCallable, Category = "Story")
	bool SelectOption(FString selected);
	UFUNCTION(BlueprintCallable, Category = "Story")
	TArray<FString> GetOptions(FString name);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void GameStart();
	UFUNCTION(BlueprintCallable, Category = "Story")
	void OptionDialog(FString name, FString option);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AActor* global;

	std::deque<Section> dialogQueue;
	bool interacting = false;
};
