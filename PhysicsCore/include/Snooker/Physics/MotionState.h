#pragma once
#include <Snooker/Physics/BallState.h>

namespace Snooker
{
enum class ClothMotion { Sliding, Rolling };
struct MotionThresholds
{
    Scalar EnterRollingSlip=1e-8;
    Scalar ExitRollingSlip=1e-6;
    bool IsValid() const
    {
        return IsFinite(EnterRollingSlip) && IsFinite(ExitRollingSlip) &&
               EnterRollingSlip>=0 && ExitRollingSlip>EnterRollingSlip;
    }
};
// Classification only: does not inject a no-slip constraint or alter ball state.
inline bool ClassifyClothMotion(const BallState& Ball, ClothMotion Previous, const MotionThresholds& Thresholds,
                                ClothMotion& Output, std::string* Error=nullptr)
{
    if(!Ball.IsValid(Error)) return false;
    if(!Thresholds.IsValid() || (Previous!=ClothMotion::Rolling && Previous!=ClothMotion::Sliding)) {
        if(Error) *Error="invalid motion thresholds or previous mode"; return false;
    }
    const auto Slip=Ball.LinearVelocityMetersPerSecond+
                    Ball.AngularVelocityRadiansPerSecond.Cross({0,0,-Ball.RadiusMeters});
    const auto Speed=std::hypot(Slip.X,Slip.Y);
    if(!IsFinite(Speed)) { if(Error) *Error="nonfinite contact slip"; return false; }
    if(Previous==ClothMotion::Sliding) Output=Speed<=Thresholds.EnterRollingSlip?ClothMotion::Rolling:Previous;
    else Output=Speed>=Thresholds.ExitRollingSlip?ClothMotion::Sliding:Previous;
    return true;
}
}
