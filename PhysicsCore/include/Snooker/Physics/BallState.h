#pragma once

#include "Snooker/Data/BallDefinition.h"

#include <cstdint>
#include <string>

namespace Snooker
{
enum class EContactKind : std::uint8_t
{
    None,
    Cloth,
    Ball,
    Cushion,
    Pocket,
};

struct FContactState
{
    EContactKind Kind = EContactKind::None;
    std::uint32_t OtherBallId = 0;
    FSnookerVector3 Normal = TableNormal;

    bool operator==(const FContactState& Other) const
    {
        return Kind == Other.Kind && OtherBallId == Other.OtherBallId && Normal.NearlyEquals(Other.Normal);
    }
};

struct BallState
{
    std::uint32_t Id = 0;
    FSnookerVector3 PositionMeters;
    FSnookerVector3 LinearVelocityMetersPerSecond;
    FSnookerVector3 AngularVelocityRadiansPerSecond;
    Meters RadiusMeters = 0.02625;
    Kilograms MassKg = 0.142;
    std::string MaterialProfileId = "standard";
    bool Sleeping = true;
    Seconds SleepTimeSeconds = 0.0;
    FContactState Contact;

    bool IsValid(std::string* OutError = nullptr) const;
    bool operator==(const BallState& Other) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Serialized, BallState& OutState, std::string* OutError = nullptr);
};
using FBallState = BallState; // Legacy alias, one implementation.
} // namespace Snooker
