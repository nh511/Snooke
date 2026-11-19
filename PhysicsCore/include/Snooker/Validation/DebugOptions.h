#pragma once

#include <cstdint>
#include <string>

namespace Snooker
{
enum class DebugFlag : std::uint32_t
{
    Trajectory = 1u << 0,
    CollisionPoints = 1u << 1,
    ContactNormals = 1u << 2,
    SleepingState = 1u << 3,
    PhysicsStepCount = 1u << 4,
};

struct DebugOptions
{
    std::uint32_t Mask = 0;

    bool Enabled(DebugFlag Flag) const;
    void Set(DebugFlag Flag, bool Enabled = true);
    bool IsEmpty() const { return Mask == 0; }
    bool IsAll() const;
    std::string Serialize() const;
    std::string Names() const;

    static bool Parse(const std::string& Value, DebugOptions& Output, std::string* Error = nullptr);
    static const char* Name(DebugFlag Flag);
};
}
