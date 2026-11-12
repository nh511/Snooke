#include "Snooker/Data/BallDefinition.h"

#include <iomanip>
#include <locale>
#include <Snooker/Serialization/Text.h>
#include <sstream>

namespace Snooker
{
namespace
{
void SetError(std::string* OutError, const char* Message)
{
    if (OutError != nullptr)
    {
        *OutError = Message;
    }
}
}

const char* BallTypeToString(const EBallType Type)
{
    switch (Type)
    {
    case EBallType::Cue: return "Cue";
    case EBallType::Red: return "Red";
    case EBallType::Yellow: return "Yellow";
    case EBallType::Green: return "Green";
    case EBallType::Brown: return "Brown";
    case EBallType::Blue: return "Blue";
    case EBallType::Pink: return "Pink";
    case EBallType::Black: return "Black";
    }
    return "Unknown";
}

bool TryParseBallType(const std::string& Value, EBallType& OutType)
{
    for (const EBallType Candidate : {EBallType::Cue, EBallType::Red, EBallType::Yellow, EBallType::Green,
                                      EBallType::Brown, EBallType::Blue, EBallType::Pink, EBallType::Black})
    {
        if (Value == BallTypeToString(Candidate))
        {
            OutType = Candidate;
            return true;
        }
    }
    return false;
}

bool BallDefinition::IsValid(std::string* OutError) const
{
    if (std::string(BallTypeToString(Type)) == "Unknown")
    {
        SetError(OutError, "unknown ball type");
        return false;
    }
    if (Id == 0)
    {
        SetError(OutError, "ball id must be non-zero");
        return false;
    }
    if (!InitialPositionMeters.IsFinite() || !IsFinite(RadiusMeters) || !IsFinite(MassKg))
    {
        SetError(OutError, "ball values must be finite");
        return false;
    }
    if (RadiusMeters <= 0.0 || MassKg <= 0.0)
    {
        SetError(OutError, "ball radius and mass must be positive");
        return false;
    }
    if (MaterialProfileId.empty() || MaterialProfileId.find_first_of(" \t\r\n") != std::string::npos)
    {
        SetError(OutError, "material profile id must be a non-empty token");
        return false;
    }
    return true;
}

bool BallDefinition::operator==(const BallDefinition& Other) const
{
    return Id == Other.Id && Type == Other.Type && InitialPositionMeters.NearlyEquals(Other.InitialPositionMeters) &&
           NearlyEqual(RadiusMeters, Other.RadiusMeters) && NearlyEqual(MassKg, Other.MassKg) &&
           MaterialProfileId == Other.MaterialProfileId;
}

std::string BallDefinition::Serialize() const
{
    std::ostringstream Stream;
    Stream.imbue(std::locale::classic());
    Stream << std::setprecision(std::numeric_limits<Scalar>::max_digits10)
           << Id << ' ' << BallTypeToString(Type) << ' '
           << InitialPositionMeters.X << ' ' << InitialPositionMeters.Y << ' ' << InitialPositionMeters.Z << ' '
           << RadiusMeters << ' ' << MassKg << ' ' << MaterialProfileId;
    return Stream.str();
}

bool BallDefinition::Deserialize(const std::string& Serialized, BallDefinition& OutDefinition, std::string* OutError)
{
    std::istringstream Stream(Serialized);
    Stream.imbue(std::locale::classic());
    std::string TypeName;
    BallDefinition Candidate;
    if (!(ReadUnsigned(Stream, Candidate.Id) && Stream >> TypeName >> Candidate.InitialPositionMeters.X >> Candidate.InitialPositionMeters.Y >>
          Candidate.InitialPositionMeters.Z >> Candidate.RadiusMeters >> Candidate.MassKg >> Candidate.MaterialProfileId) ||
        !TryParseBallType(TypeName, Candidate.Type))
    {
        SetError(OutError, "invalid ball definition serialization");
        return false;
    }
    std::string Extra;
    if (Stream >> Extra || !Candidate.IsValid(OutError))
    {
        if (Extra.size() > 0)
        {
            SetError(OutError, "unexpected data after ball definition");
        }
        return false;
    }
    OutDefinition = Candidate;
    return true;
}
} // namespace Snooker
