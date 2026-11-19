#include <Snooker/Validation/DebugOptions.h>

#include <iostream>

#define CHECK(Value) do { if (!(Value)) { std::cerr << "FAIL debug options line " << __LINE__ << ": " << #Value << '\n'; return 1; } } while (false)

int main()
{
    using namespace Snooker;
    DebugOptions options;std::string error;
    CHECK(DebugOptions::Parse("trajectory,collisions,normals,sleep,steps",options,&error));
    CHECK(options.IsAll());
    CHECK(options.Serialize()=="snooker-debug-v1 trajectory,collision-points,contact-normals,sleeping,physics-steps");
    CHECK(DebugOptions::Parse("none",options,&error));CHECK(options.IsEmpty());
    CHECK(DebugOptions::Parse("all",options,&error));CHECK(options.IsAll());
    const auto saved=options.Serialize();CHECK(!DebugOptions::Parse("unknown",options,&error));CHECK(options.Serialize()==saved);
    CHECK(!DebugOptions::Parse("trajectory,,steps",options,&error));CHECK(options.Serialize()==saved);
    std::cout<<"PASS debug flags parse, aliases, serialization and unchanged rejection\n";return 0;
}
