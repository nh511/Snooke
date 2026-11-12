#include <Snooker/Physics/ClothFriction.h>
#include <algorithm>

namespace Snooker
{
bool ApplyRollingResistance(BallState& Ball, const ClothProfile& Cloth, const ClothEnvironment& Environment,
                            Seconds Delta, std::string* Error)
{
    if (!Ball.IsValid(Error) || !Cloth.IsValid(Error)) return false;
    if (!IsFinite(Delta) || Delta<0 || !IsFinite(Environment.GravityMetersPerSecondSquared) ||
        Environment.GravityMetersPerSecondSquared<=0) {
        if(Error) *Error="invalid rolling step or gravity"; return false;
    }
    if (Ball.Sleeping || Delta==0 || Cloth.Rolling==0) return true;
    const auto Slip=Ball.LinearVelocityMetersPerSecond+
                    Ball.AngularVelocityRadiansPerSecond.Cross({0,0,-Ball.RadiusMeters});
    if (std::hypot(Slip.X,Slip.Y)>1e-9 || std::abs(Ball.LinearVelocityMetersPerSecond.Z)>1e-9) return true;
    const Scalar Speed=std::hypot(Ball.LinearVelocityMetersPerSecond.X,Ball.LinearVelocityMetersPerSecond.Y);
    if (Speed==0) return true;
    const Scalar Change=Cloth.Rolling*Environment.GravityMetersPerSecondSquared*Delta;
    if (!IsFinite(Speed) || !IsFinite(Change)) { if(Error) *Error="rolling overflow"; return false; }
    const Scalar Ratio=std::max(0.0,1-Change/Speed);
    auto Candidate=Ball;
    Candidate.LinearVelocityMetersPerSecond.X*=Ratio;
    Candidate.LinearVelocityMetersPerSecond.Y*=Ratio;
    Candidate.AngularVelocityRadiansPerSecond.X=-Candidate.LinearVelocityMetersPerSecond.Y/Candidate.RadiusMeters;
    Candidate.AngularVelocityRadiansPerSecond.Y=Candidate.LinearVelocityMetersPerSecond.X/Candidate.RadiusMeters;
    if(!Candidate.IsValid(Error)) return false;
    Ball=Candidate;
    return true;
}
}
