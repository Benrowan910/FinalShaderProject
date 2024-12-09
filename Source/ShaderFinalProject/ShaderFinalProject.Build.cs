// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShaderFinalProject : ModuleRules
{
	public ShaderFinalProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "FastNoiseGenerator", "FastNoise", "ProceduralMeshComponent"});
		PrivateDependencyModuleNames.AddRange(new string[]{"ProceduralMeshComponent"});
	}
}
