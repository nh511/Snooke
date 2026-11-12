#include "Snooker/Physics/BallState.h"

#include <iomanip>
#include <locale>
#include <Snooker/Serialization/Text.h>
#include <sstream>

namespace Snooker
{
namespace
{
const char* ContactKindToString(const EContactKind Kind)
{
    switch (Kind)
    {
    case EContactKind::None: return "None";
    case EContactKind::Cloth: return "Cloth";
    case EContactKind::Ball: return "Ball";
    case EContactKind::Cushion: return "Cushion";
    case EContactKind::Pocket: return "Pocket";
    }
    return "Unknown";
}

bool TryParseContactKind(const std::string& Value, EContactKind& OutKind)
{
    for (const EContactKind Candidate : {EContactKind::None, EContactKind::Cloth, EContactKind::Ball,
                                         EContactKind::Cushion, EContactKind::Pocket})
    {
        if (Value == ContactKindToString(Candidate))
        {
            OutKind = Candidate;
            return true;
        }
    }
    return false;
}

void SetError(std::string* OutError, const char* Message)
{
    if (OutError != nullptr)
    {
        *OutError = Message;
    }
}
}

bool BallState::IsValid(std::string* OutError) const
{
    if (std::string(ContactKindToString(Contact.Kind)) == "Unknown" ||
        (Contact.Kind == EContactKind::Ball && (Contact.OtherBallId == 0 || Contact.OtherBallId == Id)))
    {
        SetError(OutError, "invalid contact kind or contact ball id");
        return false;
    }
    if (Id == 0)
    {
        SetError(OutError, "ball state id must be non-zero");
        return false;
    }
    if (!PositionMeters.IsFinite() || !LinearVelocityMetersPerSecond.IsFinite() ||
        !AngularVelocityRadiansPerSecond.IsFinite() || !IsFinite(RadiusMeters) || !IsFinite(MassKg) ||
        !IsFinite(SleepTimeSeconds))
    {
        SetError(OutError, "ball state values must be finite");
        return false;
    }
    if (RadiusMeters <= 0.0 || MassKg <= 0.0 || SleepTimeSeconds < 0.0)
    {
        SetError(OutError, "ball state radius and mass must be positive and sleep time non-negative");
        return false;
    }
    if (MaterialProfileId.empty() || MaterialProfileId.find_first_of(" \t\r\n") != std::string::npos)
    {
        SetError(OutError, "material profile id must be a non-empty token");
        return false;
    }
    if (Contact.Kind != EContactKind::Ball && Contact.OtherBallId != 0)
    {
        SetError(OutError, "only ball contacts may have another ball id");
        return false;
    }
    if (!Contact.Normal.IsFinite())
    {
        SetError(OutError, "contact normal must be finite");
        return false;
    }
    return true;
}

bool BallState::operator==(const BallState& Other) const
{
    return Id == Other.Id && PositionMeters.NearlyEquals(Other.PositionMeters) &&
           LinearVelocityMetersPerSecond.NearlyEquals(Other.LinearVelocityMetersPerSecond) &&
           AngularVelocityRadiansPerSecond.NearlyEquals(Other.AngularVelocityRadiansPerSecond) &&
           NearlyEqual(RadiusMeters, Other.RadiusMeters) && NearlyEqual(MassKg, Other.MassKg) &&
           MaterialProfileId == Other.MaterialProfileId && Sleeping == Other.Sleeping &&
           NearlyEqual(SleepTimeSeconds, Other.SleepTimeSeconds) && Contact == Other.Contact;
}

std::string BallState::Serialize() const
{
    std::ostringstream Stream;
    Stream.imbue(std::locale::classic());
    Stream << std::setprecision(std::numeric_limits<Scalar>::max_digits10)
           << Id << ' ' << PositionMeters.X << ' ' << PositionMeters.Y << ' ' << PositionMeters.Z << ' '
           << LinearVelocityMetersPerSecond.X << ' ' << LinearVelocityMetersPerSecond.Y << ' '
           << LinearVelocityMetersPerSecond.Z << ' ' << AngularVelocityRadiansPerSecond.X << ' '
           << AngularVelocityRadiansPerSecond.Y << ' ' << AngularVelocityRadiansPerSecond.Z << ' '
           << RadiusMeters << ' ' << MassKg << ' ' << MaterialProfileId << ' ' << (Sleeping ? 1 : 0) << ' '
           << SleepTimeSeconds << ' ' << ContactKindToString(Contact.Kind) << ' ' << Contact.OtherBallId << ' '
           << Contact.Normal.X << ' ' << Contact.Normal.Y << ' ' << Contact.Normal.Z;
    return Stream.str();
}

bool BallState::Deserialize(const std::string& Serialized, BallState& OutState, std::string* OutError)
{
    std::istringstream Stream(Serialized);
    Stream.imbue(std::locale::classic());
    std::string ContactName;
    int SleepingValue = 0;
    BallState Candidate;
    if (!(ReadUnsigned(Stream, Candidate.Id) && Stream >> Candidate.PositionMeters.X >> Candidate.PositionMeters.Y >> Candidate.PositionMeters.Z >>
          Candidate.LinearVelocityMetersPerSecond.X >> Candidate.LinearVelocityMetersPerSecond.Y >>
          Candidate.LinearVelocityMetersPerSecond.Z >> Candidate.AngularVelocityRadiansPerSecond.X >>
          Candidate.AngularVelocityRadiansPerSecond.Y >> Candidate.AngularVelocityRadiansPerSecond.Z >>
          Candidate.RadiusMeters >> Candidate.MassKg >> Candidate.MaterialProfileId >> SleepingValue >>
          Candidate.SleepTimeSeconds >> ContactName >> std::ws) || !ReadUnsigned(Stream, Candidate.Contact.OtherBallId) || !(Stream >> Candidate.Contact.Normal.X >>
          Candidate.Contact.Normal.Y >> Candidate.Contact.Normal.Z) ||
        (SleepingValue != 0 && SleepingValue != 1) || !TryParseContactKind(ContactName, Candidate.Contact.Kind))
    {
        SetError(OutError, "invalid ball state serialization");
        return false;
    }
    Candidate.Sleeping = SleepingValue != 0;
    std::string Extra;
    if (Stream >> Extra || !Candidate.IsValid(OutError))
    {
        if (Extra.size() > 0)
        {
            SetError(OutError, "unexpected data after ball state");
        }
        return false;
    }
    OutState = Candidate;
    return true;
}
} // namespace Snooker
