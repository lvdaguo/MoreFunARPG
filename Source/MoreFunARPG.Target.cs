using UnrealBuildTool;

public class MoreFunARPGTarget : TargetRules
{
	public MoreFunARPGTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("MoreFunARPG");
	}
}
