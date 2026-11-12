#pragma once
#include <Snooker/Shot/ShotInput.h>

namespace Snooker
{
struct CueImpulseResult
{
    Scalar ImpulseNewtonSeconds=0;
    Vec3d ContactPointMeters;
    Vec3d LinearDeltaMetersPerSecond;
    Vec3d AngularDeltaRadiansPerSecond;
};
// Solid sphere struck by a translating cue. Current baseline requires a stationary ball.
// On failure neither Ball nor Result changes. Zero measured speed is a successful no-op.
bool ApplyCueImpulse(BallState& Ball,const ShotInput& Input,const CueProfile& Cue,
                     CueImpulseResult& Result,std::string* Error=nullptr);
}
