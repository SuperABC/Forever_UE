#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#define NOMINMAX

#include "ChronodeManager.generated.h"


class ADirectionalLight;
class ASkyAtmosphere;
class AExponentialHeightFog;

UCLASS()
class FOREVER_API AChronodeManager : public AActor {
	GENERATED_BODY()
	
public:	
	AChronodeManager();
	~AChronodeManager();

	UFUNCTION(BlueprintCallable, Category = "Chronode")
	void UpdateChronode(float chronode);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Chronode")
	ADirectionalLight* sunLight = nullptr;

	UPROPERTY(EditAnywhere, Category = "Chronode")
	ASkyAtmosphere* skyAtmosphere = nullptr;

	UPROPERTY(EditAnywhere, Category = "Chronode")
	AExponentialHeightFog* exponentialHeightFog = nullptr;

	UPROPERTY(EditAnywhere, Category = "Chronode")
	APostProcessVolume* postProcess = nullptr;

	void FindSceneComponents();
	void UpdateSunLight(float chronode);
	void UpdateSkyAtmosphere(float chronode);
	void UpdateHeightFog(float chronode);
	void UpdatePostProcess(float chronode);
};
