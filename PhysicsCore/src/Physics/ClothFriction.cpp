#include <Snooker/Physics/ClothFriction.h>
#include <algorithm>

namespace Snooker
{
bool ApplySlidingFriction(BallState& Ball, const ClothProfile& Cloth, const ClothEnvironment& Environment,
                          Seconds Delta, std::string* Error)
{
    if (!Ball.IsValid(Error) || !Cloth.IsValid(Error)) return false;
    if (!IsFinite(Delta) || Delta<0 || !IsFinite(Environment.GravityMetersPerSecondSquared) ||
        Environment.GravityMetersPerSecondSquared<=0) {
        if(Error) *Error="invalid cloth step or gravity"; return false;
    }
    if (Ball.Sleeping || Delta==0 || Cloth.Sliding==0) return true;
    const Vec3d Lever{0,0,-Ball.RadiusMeters};
    auto Slip=Ball.LinearVelocityMetersPerSecond+Ball.AngularVelocityRadiansPerSecond.Cross(Lever);
    Slip.Z=0;
    const Scalar Speed=std::hypot(Slip.X,Slip.Y);
    if (!IsFinite(Speed)) { if(Error) *Error="nonfinite contact slip"; return false; }
    if (Speed==0) return true;
    // I=2/5*m*r^2: impulse changes contact slip by 7/(2*m).
    const Scalar MaxDeltaV=Cloth.Sliding*Environment.GravityMetersPerSecondSquared*Delta;
    if (!IsFinite(MaxDeltaV)) { if(Error) *Error="friction impulse overflow"; return false; }
    const Scalar DeltaV=std::min(MaxDeltaV,Speed*(2.0/7));
    const Vec3d LinearChange=Slip*(-DeltaV/Speed);
    const Vec3d AngularChange=Lever.Cross(LinearChange)*(2.5/(Ball.RadiusMeters*Ball.RadiusMeters));
    auto Candidate=Ball;
    Candidate.LinearVelocityMetersPerSecond+=LinearChange;
    Candidate.AngularVelocityRadiansPerSecond+=AngularChange;
    if(!Candidate.IsValid(Error)) return false;
    Ball=Candidate;
    return true;
}
}
