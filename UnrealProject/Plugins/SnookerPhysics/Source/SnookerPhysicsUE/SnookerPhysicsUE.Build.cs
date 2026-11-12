using UnrealBuildTool;
public class SnookerPhysicsUE : ModuleRules
{
    public SnookerPhysicsUE(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "SnookerPhysicsCore" });
    }
}
