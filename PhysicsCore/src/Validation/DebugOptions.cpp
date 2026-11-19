#include <Snooker/Validation/DebugOptions.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <sstream>
#include <vector>

namespace Snooker
{
namespace
{
constexpr std::array<DebugFlag, 5> Flags = {
    DebugFlag::Trajectory,
    DebugFlag::CollisionPoints,
    DebugFlag::ContactNormals,
    DebugFlag::SleepingState,
    DebugFlag::PhysicsStepCount,
};

std::string Lower(std::string Value)
{
    std::transform(Value.begin(), Value.end(), Value.begin(), [](unsigned char Character) {
        return static_cast<char>(std::tolower(Character));
    });
    return Value;
}

bool Fail(std::string* Error, const char* Message)
{
    if (Error) *Error = Message;
    return false;
}

std::uint32_t Bit(DebugFlag Flag)
{
    return static_cast<std::uint32_t>(Flag);
}
}

const char* DebugOptions::Name(DebugFlag Flag)
{
    switch (Flag) {
    case DebugFlag::Trajectory: return "trajectory";
    case DebugFlag::CollisionPoints: return "collision-points";
    case DebugFlag::ContactNormals: return "contact-normals";
    case DebugFlag::SleepingState: return "sleeping";
    case DebugFlag::PhysicsStepCount: return "physics-steps";
    }
    return "unknown";
}

bool DebugOptions::Enabled(DebugFlag Flag) const
{
    return (Mask & Bit(Flag)) != 0;
}

void DebugOptions::Set(DebugFlag Flag, bool Value)
{
    if (Value) Mask |= Bit(Flag);
    else Mask &= ~Bit(Flag);
}

bool DebugOptions::IsAll() const
{
    std::uint32_t All = 0;
    for (const auto Flag : Flags) All |= Bit(Flag);
    return Mask == All;
}

std::string DebugOptions::Names() const
{
    std::ostringstream Output;
    bool First = true;
    for (const auto Flag : Flags) {
        if (!Enabled(Flag)) continue;
        if (!First) Output << ',';
        Output << Name(Flag);
        First = false;
    }
    return Output.str();
}

std::string DebugOptions::Serialize() const
{
    return "snooker-debug-v1 " + Names();
}

bool DebugOptions::Parse(const std::string& Value, DebugOptions& Output, std::string* Error)
{
    DebugOptions Candidate;
    if (Value.empty()) return Fail(Error, "debug option list is empty");
    std::size_t Start = 0;
    while (Start <= Value.size()) {
        const auto End = Value.find(',', Start);
        auto Token = Lower(Value.substr(Start, End == std::string::npos ? std::string::npos : End - Start));
        if (Token.empty()) return Fail(Error, "debug option contains an empty token");
        if (Token == "all") {
            for (const auto Flag : Flags) Candidate.Set(Flag);
        } else if (Token == "none") {
            Candidate.Mask = 0;
        } else if (Token == "collisions" || Token == "contacts") {
            Candidate.Set(DebugFlag::CollisionPoints);
        } else if (Token == "normals" || Token == "contact-normal") {
            Candidate.Set(DebugFlag::ContactNormals);
        } else if (Token == "sleep" || Token == "sleeping-state") {
            Candidate.Set(DebugFlag::SleepingState);
        } else if (Token == "steps" || Token == "step-count") {
            Candidate.Set(DebugFlag::PhysicsStepCount);
        } else if (Token == "trajectory" || Token == "paths") {
            Candidate.Set(DebugFlag::Trajectory);
        } else {
            return Fail(Error, "unknown debug option");
        }
        if (End == std::string::npos) break;
        Start = End + 1;
    }
    Output = Candidate;
    return true;
}
}
