#include <Snooker/Physics/Touching.h>
#include <algorithm>

namespace Snooker
{
bool QueryTouching(const BallState& First,const BallState& Second,const TouchingThresholds& Thresholds,
                   TouchingResult& Output,std::string* Error)
{
    const auto fail=[&](const char* message){if(Error)*Error=message;return false;};
    if(!First.IsValid(Error) || !Second.IsValid(Error))return false;
    if(First.Id==Second.Id || !IsFinite(Thresholds.GapTolerance) || Thresholds.GapTolerance<0 ||
       !IsFinite(Thresholds.NormalSpeedTolerance) || Thresholds.NormalSpeedTolerance<0 ||
       Thresholds.GapTolerance>=std::min(First.RadiusMeters,Second.RadiusMeters))return fail("invalid touching ids or thresholds");
    const Vec3d separation=Second.PositionMeters-First.PositionMeters;
    const Scalar distance=std::hypot(separation.X,separation.Y,separation.Z), radius=First.RadiusMeters+Second.RadiusMeters;
    if(!separation.IsFinite() || !IsFinite(distance) || !IsFinite(radius))return fail("touching geometry overflow");
    TouchingResult r;r.SignedGap=distance-radius;
    if(distance==0) return fail("coincident centers have no rule-facing contact normal");
    r.Normal=separation*(1/distance);
    const auto relative=Second.LinearVelocityMetersPerSecond-First.LinearVelocityMetersPerSecond;
    r.RelativeNormalSpeed=relative.Dot(r.Normal);
    if(!relative.IsFinite() || !IsFinite(r.RelativeNormalSpeed))return fail("touching velocity overflow");
    r.Relation=r.SignedGap>Thresholds.GapTolerance?TouchingRelation::Separated:
        r.SignedGap < -Thresholds.GapTolerance?TouchingRelation::Overlapping:TouchingRelation::Touching;
    r.Motion=r.RelativeNormalSpeed < -Thresholds.NormalSpeedTolerance?RelativeNormalMotion::Approaching:
        r.RelativeNormalSpeed>Thresholds.NormalSpeedTolerance?RelativeNormalMotion::Separating:RelativeNormalMotion::Resting;
    Output=r;return true;
}
}
