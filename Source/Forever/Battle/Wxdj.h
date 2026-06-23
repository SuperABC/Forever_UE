#pragma once

#define NOMINMAX

#include "CoreMinimal.h"

#include "controller.h"

#include "Wxdj.generated.h"


UCLASS(Blueprintable, BlueprintType)
class FOREVER_API UWxdj : public UObject {
	GENERATED_BODY()

public:
	UWxdj();
	~UWxdj();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void AddCultivator(const FString& name, const FString& file,
		int hp, int atk, int mp, int agi, int recover);

	UFUNCTION(BlueprintCallable, Category = "Battle")
	FString GetLog();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	bool NextStage();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void SelectMove(int idx, int subject, int object);

private:
	std::shared_ptr<Controller> controller;
	std::vector<std::pair<std::shared_ptr<Action>, int>> currentActions;

	int turnCount;
	int turnStage;
};
