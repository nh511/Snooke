using UnrealBuildTool;
public class SnookerPhysicsCore : ModuleRules
{
    public SnookerPhysicsCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;
        PrivateDependencyModuleNames.Add("Core");
        // Only module startup exists here. Shared source integration is COR-009.
    }
}
