#include "Actor/StartBase.h"


using namespace std;

AStartBase::AStartBase() {
	PrimaryActorTick.bCanEverTick = true;
}

AStartBase::~AStartBase() {

}

void AStartBase::BeginPlay() {
	Super::BeginPlay();
}

void AStartBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

TArray<FString> AStartBase::GetDllPaths() {
	TArray<FString> paths;

	auto dllPaths = Config::GetDllPaths();
	for (auto dllPath : dllPaths) {
		paths.Add(FString(UTF8_TO_TCHAR(dllPath.data())));
	}

	return paths;
}

TArray<FString> AStartBase::GetModPaths() {
	TArray<FString> paths;

	auto modPaths = Config::GetMods();
	for (auto modPath : modPaths) {
		paths.Add(FString(UTF8_TO_TCHAR(modPath.data())));
	}

	return paths;
}

void AStartBase::AddDllPath(FString path) {
	Config::AddDllPath(string(TCHAR_TO_UTF8(*path)));
}

void AStartBase::RemoveDllPath(FString path) {
	Config::RemoveDllPath(string(TCHAR_TO_UTF8(*path)));
}

void AStartBase::GetChecks(FString type, TArray<FString>& mods, TArray<bool>& enables) {
	auto checks = Config::GetChecks(string(TCHAR_TO_UTF8(*type)));
	mods.Reserve(checks.size());
	int i = 0;
	for (auto check : checks) {
		mods[i] = FString(UTF8_TO_TCHAR(check.first.data()));
		enables[i] = check.second;
		i++;
	}
}

void AStartBase::CheckMod(FString type, FString mod, bool enabled) {
	Config::CheckMod(string(TCHAR_TO_UTF8(*type)),
		string(TCHAR_TO_UTF8(*type)), enabled);
}

TArray<FString> AStartBase::GetResourcePaths() {
	TArray<FString> paths;

	auto resourcePaths = Config::GetResourcePaths();
	for (auto resourcePath : resourcePaths) {
		paths.Add(FString(UTF8_TO_TCHAR(resourcePath.data())));
	}

	return paths;
}

TArray<FString> AStartBase::GetResources() {
	TArray<FString> paths;

	auto layoutPaths = Config::GetLayouts();
	for (auto layoutPath : layoutPaths) {
		paths.Add(FString(UTF8_TO_TCHAR(layoutPath.data())));
	}

	auto jobPaths = Config::GetJobs();
	for (auto [jobName, jobPath] : jobPaths) {
		paths.Add(FString(UTF8_TO_TCHAR(jobPath.data())));
	}

	auto characterPaths = Config::GetCharacters();
	for (auto characterPath : characterPaths) {
		paths.Add(FString(UTF8_TO_TCHAR(characterPath.data())));
	}

	return paths;
}

void AStartBase::AddResourcePath(FString path) {
	Config::AddResourcePath(string(TCHAR_TO_UTF8(*path)));
}

void AStartBase::RemoveResourcePath(FString path) {
	Config::RemoveResourcePath(string(TCHAR_TO_UTF8(*path)));
}

FString AStartBase::GetScriptPath() {
	return FString(UTF8_TO_TCHAR(Config::GetScript().data()));
}

void AStartBase::SetScriptPath(FString path) {
	Config::SetScript(string(TCHAR_TO_UTF8(*path)));
}