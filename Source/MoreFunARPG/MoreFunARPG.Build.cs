// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MoreFunARPG : ModuleRules
{
	public MoreFunARPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[]
			{ "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}