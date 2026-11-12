using UnrealBuildTool;
public class SnookerGame : ModuleRules
{
    public SnookerGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "EnhancedInput", "InputCore", "UMG", "SnookerPhysicsUE" });
    }
}
