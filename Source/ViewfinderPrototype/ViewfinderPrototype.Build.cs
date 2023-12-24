// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ViewfinderPrototype : ModuleRules
{
	public ViewfinderPrototype(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "UMG", "ProceduralMeshComponent" });
	}
}
