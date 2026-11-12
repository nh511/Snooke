#pragma once
#include <Snooker/Data/ClothProfile.h>
#include <Snooker/Physics/BallState.h>
#include <algorithm>

namespace Snooker
{
// PreviousPosition is the center position immediately before this step's integration.
// SleepTimeSeconds is the complete persistent timer; no hidden detector state exists.
inline bool UpdateSleep(BallState& Ball,const Vec3d& PreviousPosition,const ClothProfile& Cloth,
                         Seconds Delta,std::string* Error=nullptr)
{
    if(!Ball.IsValid(Error) || !Cloth.IsValid(Error)) return false;
    const auto displacement=Ball.PositionMeters-PreviousPosition;
    const Scalar distance=std::hypot(displacement.X,displacement.Y,displacement.Z);
    const auto& linear=Ball.LinearVelocityMetersPerSecond;
    const auto& angular=Ball.AngularVelocityRadiansPerSecond;
    if(!PreviousPosition.IsFinite() || !IsFinite(distance) || !IsFinite(Delta) || Delta<0) {
        if(Error) *Error="invalid previous sleep position or step"; return false;
    }
    if(Delta==0) return true;
    const bool quiet=distance<=Cloth.SleepPositionTolerance &&
        std::hypot(linear.X,linear.Y,linear.Z)<=Cloth.SleepLinearSpeed &&
        std::hypot(angular.X,angular.Y,angular.Z)<=Cloth.SleepAngularSpeed;
    auto candidate=Ball;
    if(!quiet) { candidate.Sleeping=false; candidate.SleepTimeSeconds=0; }
    else {
        // Bounded addition avoids a long-running timer overflowing.
        candidate.SleepTimeSeconds=std::min(Cloth.SleepDuration,
            std::min(candidate.SleepTimeSeconds,Cloth.SleepDuration)+std::min(Delta,Cloth.SleepDuration));
        if(candidate.SleepTimeSeconds>=Cloth.SleepDuration) {
            candidate.Sleeping=true;
            candidate.LinearVelocityMetersPerSecond={};
            candidate.AngularVelocityRadiansPerSecond={};
        }
    }
    if(!candidate.IsValid(Error)) return false;
    Ball=candidate;
    return true;
}
}
