using UnrealBuildTool;

public class MoreFunARPGEditorTarget : TargetRules
{
	public MoreFunARPGEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("MoreFunARPG");
	}
}
