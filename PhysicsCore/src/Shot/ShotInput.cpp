#include "Snooker/Shot/ShotInput.h"

#include <iomanip>
#include <algorithm>
#include <locale>
#include <Snooker/Serialization/Text.h>
#include <sstream>

namespace Snooker
{
namespace
{
void SetError(std::string* OutError, const char* Message)
{
    if (OutError != nullptr) *OutError = Message;
}
}

bool ShotInput::IsValid(std::string* OutError) const
{
    if (CueBallId == 0 || CueProfileId.empty() || CueProfileId.find_first_of(" \t\r\n") != std::string::npos)
    {
        SetError(OutError, "shot must identify a cue ball and cue profile token");
        return false;
    }
    const Scalar DirectionLength=std::hypot(CueDirection.X,CueDirection.Y,CueDirection.Z);
    if (!CueDirection.IsFinite() || !IsFinite(DirectionLength) || DirectionLength <= 1e-9 || !StrikePointOnBall.IsFinite() ||
        !CueTipOffsetMeters.IsFinite() || !IsFinite(PullbackDistanceMeters) || !IsFinite(ForwardSpeedMetersPerSecond) ||
        !IsFinite(InputTimeSeconds))
    {
        SetError(OutError, "shot values must be finite and direction must be non-zero");
        return false;
    }
    if (StrikePointOnBall.LengthSquared() > 1.0 || PullbackDistanceMeters < 0.0 ||
        ForwardSpeedMetersPerSecond < 0.0 || InputTimeSeconds < 0.0)
    {
        SetError(OutError, "shot contains an out-of-range strike point or negative input value");
        return false;
    }
    const Vec3d Direction=CueDirection*(1/DirectionLength);
    const Scalar OffsetLength=std::hypot(CueTipOffsetMeters.X,CueTipOffsetMeters.Y,CueTipOffsetMeters.Z);
    if(!IsFinite(OffsetLength) || std::abs(StrikePointOnBall.Dot(Direction))>1e-9 ||
       std::abs(CueTipOffsetMeters.Dot(Direction))>1e-9*std::max(1.0,OffsetLength)) {
        SetError(OutError,"strike point and tip offset must be transverse to cue direction"); return false;
    }
    return true;
}

bool ShotInput::ValidateFor(const CueProfile& Cue,const BallState& Ball,std::string* Error) const
{
    if(!IsValid(Error) || !Cue.IsValid(Error) || !Ball.IsValid(Error)) return false;
    if(CueProfileId!=Cue.Id || CueBallId!=Ball.Id) {
        SetError(Error,"shot ball/profile id mismatch"); return false;
    }
    const Vec3d Expected=StrikePointOnBall*Ball.RadiusMeters;
    if(!Expected.NearlyEquals(CueTipOffsetMeters,1e-10)) {
        SetError(Error,"normalized strike point and metric tip offset disagree"); return false;
    }
    return Cue.AllowsInput(PullbackDistanceMeters,ForwardSpeedMetersPerSecond,
        std::hypot(CueTipOffsetMeters.X,CueTipOffsetMeters.Y,CueTipOffsetMeters.Z),Ball.RadiusMeters,Error);
}

bool ShotInput::operator==(const ShotInput& Other) const
{
    return CueBallId == Other.CueBallId && CueProfileId == Other.CueProfileId &&
           CueDirection.NearlyEquals(Other.CueDirection) && StrikePointOnBall.NearlyEquals(Other.StrikePointOnBall) &&
           NearlyEqual(PullbackDistanceMeters, Other.PullbackDistanceMeters) &&
           NearlyEqual(ForwardSpeedMetersPerSecond, Other.ForwardSpeedMetersPerSecond) &&
           CueTipOffsetMeters.NearlyEquals(Other.CueTipOffsetMeters) && NearlyEqual(InputTimeSeconds, Other.InputTimeSeconds);
}

std::string ShotInput::Serialize() const
{
    std::ostringstream Stream;
    Stream.imbue(std::locale::classic());
    Stream << std::setprecision(std::numeric_limits<Scalar>::max_digits10)
           << CueBallId << ' ' << CueProfileId << ' ' << CueDirection.X << ' ' << CueDirection.Y << ' ' << CueDirection.Z << ' '
           << StrikePointOnBall.X << ' ' << StrikePointOnBall.Y << ' ' << StrikePointOnBall.Z << ' '
           << PullbackDistanceMeters << ' ' << ForwardSpeedMetersPerSecond << ' ' << CueTipOffsetMeters.X << ' '
           << CueTipOffsetMeters.Y << ' ' << CueTipOffsetMeters.Z << ' ' << InputTimeSeconds;
    return Stream.str();
}

bool ShotInput::Deserialize(const std::string& Serialized, ShotInput& OutInput, std::string* OutError)
{
    std::istringstream Stream(Serialized);
    Stream.imbue(std::locale::classic());
    ShotInput Candidate;
    if (!(ReadUnsigned(Stream, Candidate.CueBallId) && Stream >> Candidate.CueProfileId >> Candidate.CueDirection.X >> Candidate.CueDirection.Y >>
          Candidate.CueDirection.Z >> Candidate.StrikePointOnBall.X >> Candidate.StrikePointOnBall.Y >>
          Candidate.StrikePointOnBall.Z >> Candidate.PullbackDistanceMeters >> Candidate.ForwardSpeedMetersPerSecond >>
          Candidate.CueTipOffsetMeters.X >> Candidate.CueTipOffsetMeters.Y >> Candidate.CueTipOffsetMeters.Z >>
          Candidate.InputTimeSeconds))
    {
        SetError(OutError, "invalid shot input serialization");
        return false;
    }
    std::string Extra;
    if (Stream >> Extra || !Candidate.IsValid(OutError))
    {
        if (!Extra.empty()) SetError(OutError, "unexpected data after shot input");
        return false;
    }
    OutInput = Candidate;
    return true;
}
} // namespace Snooker
