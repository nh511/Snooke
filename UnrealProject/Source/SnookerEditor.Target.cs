using UnrealBuildTool;
using System.Collections.Generic;
public class SnookerEditorTarget : TargetRules
{
    public SnookerEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        ExtraModuleNames.AddRange(new string[] { "SnookerGame", "SnookerEditor" });
    }
}
