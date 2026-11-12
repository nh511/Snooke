#include "Snooker/Shot/CueProfile.h"
#include <iomanip>
#include <locale>
#include <sstream>
#include <algorithm>

namespace Snooker
{
bool CueProfile::IsValid(std::string* OutError) const
{
    if (Id.empty() || Id.find_first_of(" \t\r\n") != std::string::npos ||
        ParameterVersion.empty() || ParameterVersion.find_first_of(" \t\r\n") != std::string::npos)
    {
        if (OutError != nullptr) *OutError = "cue profile id must be a non-empty token";
        return false;
    }
    if (!IsFinite(LengthMeters) || !IsFinite(MassKg) || !IsFinite(TipRadiusMeters) ||
        !IsFinite(MaxPullbackMeters) || !IsFinite(MaxImpulseNewtonSeconds) || LengthMeters <= 0.0 ||
        MassKg <= 0.0 || TipRadiusMeters <= 0.0 || MaxPullbackMeters < 0.0 || MaxImpulseNewtonSeconds < 0.0 ||
        !IsFinite(MaxForwardSpeedMetersPerSecond) || MaxForwardSpeedMetersPerSecond<=0 ||
        !IsFinite(MaxOffsetRadiusRatio) || MaxOffsetRadiusRatio<0 || MaxOffsetRadiusRatio>=1 ||
        !IsFinite(TipRestitution) || TipRestitution<0 || TipRestitution>1)
    {
        if (OutError != nullptr) *OutError = "cue profile values must be finite and within physical bounds";
        return false;
    }
    return true;
}
bool CueProfile::AllowsInput(Meters Pullback,Scalar ForwardSpeed,Meters Offset,Meters BallRadius,std::string* Error) const
{
    if(!IsValid(Error)) return false;
    if(!IsFinite(Pullback) || Pullback<0 || Pullback>MaxPullbackMeters ||
       !IsFinite(ForwardSpeed) || ForwardSpeed<0 || ForwardSpeed>MaxForwardSpeedMetersPerSecond ||
       !IsFinite(Offset) || Offset<0 || !IsFinite(BallRadius) || BallRadius<=TipRadiusMeters ||
       Offset>std::min(MaxOffsetRadiusRatio*BallRadius,BallRadius-TipRadiusMeters)) {
        if(Error) *Error="cue input exceeds pullback/speed/tip reach limits"; return false;
    }
    return true;
}
std::string CueProfile::Serialize() const
{
    std::ostringstream s; s.imbue(std::locale::classic());
    s<<std::setprecision(std::numeric_limits<Scalar>::max_digits10)
     <<"snooker-cue-v1 "<<Id<<' '<<ParameterVersion<<' '<<LengthMeters<<' '<<MassKg<<' '<<TipRadiusMeters<<' '
     <<MaxPullbackMeters<<' '<<MaxImpulseNewtonSeconds<<' '<<MaxForwardSpeedMetersPerSecond<<' '
     <<MaxOffsetRadiusRatio<<' '<<TipRestitution;
    return s.str();
}
bool CueProfile::Deserialize(const std::string& Text,CueProfile& Output,std::string* Error)
{
    std::istringstream s(Text); s.imbue(std::locale::classic()); CueProfile c; std::string magic;
    if(!(s>>magic>>c.Id>>c.ParameterVersion>>c.LengthMeters>>c.MassKg>>c.TipRadiusMeters>>c.MaxPullbackMeters
          >>c.MaxImpulseNewtonSeconds>>c.MaxForwardSpeedMetersPerSecond>>c.MaxOffsetRadiusRatio>>c.TipRestitution) || magic!="snooker-cue-v1") {
        if(Error) *Error="invalid cue schema or fields"; return false;
    }
    s>>std::ws;
    if(!s.eof()) { if(Error) *Error="unexpected data after cue profile"; return false; }
    if(!c.IsValid(Error)) return false;
    Output=std::move(c); return true;
}
} // namespace Snooker
