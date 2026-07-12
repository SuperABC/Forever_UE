#include "ChronodeManager.h"

#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/exponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Kismet/GameplayStatics.h"


using namespace std;

AChronodeManager::AChronodeManager() {
	PrimaryActorTick.bCanEverTick = true;
}

AChronodeManager::~AChronodeManager() {

}

void AChronodeManager::BeginPlay() {
	Super::BeginPlay();

	FindSceneComponents();
}

void AChronodeManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AChronodeManager::UpdateChronode(float chronode) {
	UpdateSunLight(chronode);
	UpdateSkyAtmosphere(chronode);
	UpdateHeightFog(chronode);
	UpdatePostProcess(chronode);
}

void AChronodeManager::FindSceneComponents() {
	if (!sunLight) {
		sunLight = Cast<ADirectionalLight>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass()));
	}

	if (!skyAtmosphere) {
		skyAtmosphere = Cast<ASkyAtmosphere>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ASkyAtmosphere::StaticClass()));
	}

	if (!exponentialHeightFog) {
		exponentialHeightFog = Cast<AExponentialHeightFog>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AExponentialHeightFog::StaticClass()));
	}

	if (!postProcess) {
		postProcess = Cast<APostProcessVolume>(
			UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()));
	}
}

void AChronodeManager::UpdateSunLight(float chronode) {
	if (!sunLight) return;

	UDirectionalLightComponent* sunComponent = Cast<UDirectionalLightComponent>(sunLight->GetLightComponent());
	if (!sunComponent) return;

	sunLight->SetActorRotation(FRotator(FMath::Lerp(90.0f, -270.0f, chronode), 0.0f, 0.0f));
	sunComponent->SetIntensity(FMath::Clamp(FMath::Sin(chronode * PI), 0.0f, 1.0f) * 100000.0f);

	FLinearColor lightColor;
	// 凌晨至清晨（0~0.2）：橙红色朝霞，太阳刚升起时的低角度暖色调
	if (chronode < 0.2f)
		lightColor = FLinearColor(1.0f, 0.5f, 0.2f);
	// 清晨过渡段（0.2~0.3）：橙黄色，日出后光线逐渐变暖变亮
	else if (chronode < 0.3f)
		lightColor = FLinearColor(1.0f, 0.7f, 0.3f);
	// 白天正午段（0.3~0.7）：接近白色的暖白光，模拟正午日光
	else if (chronode < 0.7f)
		lightColor = FLinearColor(1.0f, 0.95f, 0.9f);
	// 黄昏过渡段（0.7~0.8）：橙红色夕阳，与清晨对称的低角度暖色调
	else if (chronode < 0.8f)
		lightColor = FLinearColor(1.0f, 0.5f, 0.2f);
	// 夜间（0.8~1.0）：偏蓝紫色的月光/环境光，模拟夜间低照度冷色调
	else
		lightColor = FLinearColor(0.5f, 0.5f, 0.8f);
	sunComponent->SetLightColor(lightColor);
}

void AChronodeManager::UpdateSkyAtmosphere(float chronode) {
	if (!skyAtmosphere) return;

	USkyAtmosphereComponent* atmosphrereComponent = skyAtmosphere->GetComponent();
	if (atmosphrereComponent) {
		float sunIntensity = FMath::Clamp(FMath::Sin(chronode * PI), 0.0f, 1.0f) * 100000.0f;
		atmosphrereComponent->SetMultiScatteringFactor(FMath::Clamp(sunIntensity / 50000.0f, 0.1f, 1.0f));
	}
}

void AChronodeManager::UpdateHeightFog(float chronode) {
	if (!exponentialHeightFog) return;

	UExponentialHeightFogComponent* fogComponent =
		exponentialHeightFog->GetComponentByClass<UExponentialHeightFogComponent>();
	if (fogComponent) {
		float fogDensity = 0.0001f + FMath::Pow(1.0f - FMath::Abs(chronode - 0.5f) * 2.0f, 2.0f) * 0.0001f;
		fogComponent->SetFogDensity(fogDensity);
	}
}

void AChronodeManager::UpdatePostProcess(float chronode) {
	if (!postProcess) return;

	float globalIntensity = -FMath::Cos(chronode * PI * 2);
	postProcess->Settings.AutoExposureMinBrightness = globalIntensity * 10;
}

