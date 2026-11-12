#pragma once

#include "Snooker/Shot/CueProfile.h"
#include "Snooker/Physics/BallState.h"

#include <string>

namespace Snooker
{
struct ShotInput
{
    std::uint32_t CueBallId = 0;
    std::string CueProfileId = "standard";
    FSnookerVector3 CueDirection = {1.0, 0.0, 0.0};
    FSnookerVector3 StrikePointOnBall = {0.0, 0.0, 0.0};
    Meters PullbackDistanceMeters = 0.0;
    Scalar ForwardSpeedMetersPerSecond = 0.0;
    FSnookerVector3 CueTipOffsetMeters;
    Seconds InputTimeSeconds = 0.0;

    bool IsValid(std::string* OutError = nullptr) const;
    bool ValidateFor(const CueProfile& Cue, const BallState& Ball, std::string* Error=nullptr) const;
    bool operator==(const ShotInput& Other) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Serialized, ShotInput& OutInput, std::string* OutError = nullptr);
};
using FShotInput = ShotInput;
} // namespace Snooker
