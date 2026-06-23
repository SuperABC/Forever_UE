// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Forever : ModuleRules
{
	public Forever(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableExceptions = true;
        bUseRTTI = true;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore",
			"ProceduralMeshComponent",
			"Water"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { "FreeType2", "ImageWrapper", "Projects" });

		AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "UElibPNG");

		PublicIncludePaths.AddRange(new string[] {
			"Forever",
            "Dependence/common",
            "Dependence/",
            "Core/common",
            "Core",
			"WXDJ",
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
