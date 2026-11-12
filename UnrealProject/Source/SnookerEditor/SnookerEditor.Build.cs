using UnrealBuildTool;
public class SnookerEditor : ModuleRules
{
    public SnookerEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UnrealEd", "SnookerGame", "SnookerPhysicsUE" });
    }
}
