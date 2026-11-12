#pragma once

#include "Snooker/Math/SnookerUnits.h"

#include <string>

namespace Snooker
{
struct CueProfile
{
    std::string Id = "standard";
    Meters LengthMeters = 1.45;
    Kilograms MassKg = 0.50;
    Meters TipRadiusMeters = 0.006;
    Meters MaxPullbackMeters = 0.25;
    Scalar MaxImpulseNewtonSeconds = 8.0;
    std::string ParameterVersion = "cue-engineering-v1";
    Scalar MaxForwardSpeedMetersPerSecond = 5.0;
    Scalar MaxOffsetRadiusRatio = 0.7;
    Scalar TipRestitution = 0.5;

    bool IsValid(std::string* OutError = nullptr) const;
    bool AllowsInput(Meters Pullback, Scalar ForwardSpeed, Meters Offset, Meters BallRadius,
                     std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,CueProfile& Output,std::string* Error=nullptr);
};
using FCueProfile = CueProfile;
} // namespace Snooker
