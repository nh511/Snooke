using UnrealBuildTool;
using System.Collections.Generic;
public class SnookerTarget : TargetRules
{
    public SnookerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        ExtraModuleNames.Add("SnookerGame");
    }
}
