#pragma once

#include "Snooker/Math/SnookerUnits.h"

#include <cstdint>
#include <string>

namespace Snooker
{
enum class EBallType : std::uint8_t
{
    Cue,
    Red,
    Yellow,
    Green,
    Brown,
    Blue,
    Pink,
    Black,
};

const char* BallTypeToString(EBallType Type);
bool TryParseBallType(const std::string& Value, EBallType& OutType);

struct BallDefinition
{
    std::uint32_t Id = 0;
    EBallType Type = EBallType::Cue;
    FSnookerVector3 InitialPositionMeters;
    Meters RadiusMeters = 0.02625;
    Kilograms MassKg = 0.142;
    std::string MaterialProfileId = "standard";

    bool IsValid(std::string* OutError = nullptr) const;
    bool operator==(const BallDefinition& Other) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Serialized, BallDefinition& OutDefinition, std::string* OutError = nullptr);
};
using FBallDefinition = BallDefinition; // Compatibility with the original preparation API.
} // namespace Snooker
