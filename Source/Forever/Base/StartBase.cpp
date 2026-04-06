#include "StartBase.h"
#include "DesktopPlatformModule.h"


using namespace std;

AStartBase::AStartBase() {
	PrimaryActorTick.bCanEverTick = true;
}

AStartBase::~AStartBase() {

}

void AStartBase::BeginPlay() {
	Super::BeginPlay();

	Config::ReadConfig(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/config.json");
}

void AStartBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

FString AStartBase::SelectFolder() {
	FString SelectedPath;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform) {
		void* ParentWindowHandle = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
		DesktopPlatform->OpenDirectoryDialog(
			ParentWindowHandle,
			TEXT("Select Folder"),
			FPaths::ProjectDir(),
			SelectedPath);
	}
	return SelectedPath;
}

FString AStartBase::SelectFile() {
	FString SelectedFilePath;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform) {
		void* ParentWindowHandle = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
		TArray<FString> OutFiles;
		DesktopPlatform->OpenFileDialog(
			ParentWindowHandle,
			TEXT("Select File"),
			FPaths::ProjectDir(),
			TEXT(""),
			TEXT("JSON files (*.json)|*.json|All files (*.*)|*.*"),
			EFileDialogFlags::None,
			OutFiles
		);

		if (OutFiles.Num() > 0) {
			SelectedFilePath = OutFiles[0];
		}
	}
	return SelectedFilePath;
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
	mods.Empty();
	enables.Empty();
	int i = 0;
	for (auto check : checks) {
		mods.Add(FString(UTF8_TO_TCHAR(check.first.data())));
		enables.Add(check.second);
		i++;
	}
}

void AStartBase::CheckMod(FString type, FString mod, bool enabled) {
	Config::CheckMod(string(TCHAR_TO_UTF8(*type)),
		string(TCHAR_TO_UTF8(*mod)), enabled);
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

	auto scriptPaths = Config::GetScripts();
	for (auto [scriptName, scriptPath] : scriptPaths) {
		paths.Add(FString(UTF8_TO_TCHAR(scriptPath.data())));
	}

	auto actionPaths = Config::GetActions();
	for (auto actionPath : actionPaths) {
		paths.Add(FString(UTF8_TO_TCHAR(actionPath.data())));
	}

	return paths;
}

void AStartBase::AddResourcePath(FString path) {
	Config::AddResourcePath(string(TCHAR_TO_UTF8(*path)));
}

void AStartBase::RemoveResourcePath(FString path) {
	Config::RemoveResourcePath(string(TCHAR_TO_UTF8(*path)));
}

TArray<FString> AStartBase::GetScriptPaths() {
	TArray<FString> paths;

	auto stories = Config::GetStories();
	for (auto story : stories) {
		paths.Add(FString(UTF8_TO_TCHAR(story.data())));
	}

	return paths;
}

void AStartBase::AddScriptPath(FString path) {
	Config::AddScript(string(TCHAR_TO_UTF8(*path)));
}

void AStartBase::RemoveScriptPath(FString path) {
	Config::RemoveScript(string(TCHAR_TO_UTF8(*path)));
}

void AStartBase::SaveConfig() {
	Config::WriteConfig(string(TCHAR_TO_UTF8(*FPaths::ProjectDir())) + "Source/Resources/config.json");
}
