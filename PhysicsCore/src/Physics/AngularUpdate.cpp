#include <Snooker/Physics/ClothFriction.h>
#include <algorithm>
#include <cmath>

namespace Snooker
{
bool ApplyAngularUpdate(BallState& Ball, const ClothProfile& Cloth, const ClothEnvironment& Environment,
                        Seconds Delta, std::string* Error)
{
    auto Candidate=Ball;
    if(!ApplySlidingFriction(Candidate,Cloth,Environment,Delta,Error)) return false;
    if(!Candidate.Sleeping && Delta>0 && Cloth.Spin>0) {
        // Spin is the engineering angular deceleration in rad/s^2, not a linear friction coefficient.
        const Scalar Change=Cloth.Spin*Delta;
        if(!IsFinite(Change)) { if(Error) *Error="side-spin damping overflow"; return false; }
        const Scalar Spin=Candidate.AngularVelocityRadiansPerSecond.Z;
        Candidate.AngularVelocityRadiansPerSecond.Z=std::copysign(std::max(0.0,std::abs(Spin)-Change),Spin);
    }
    if(!Candidate.IsValid(Error)) return false;
    Ball=Candidate;
    return true;
}
}
