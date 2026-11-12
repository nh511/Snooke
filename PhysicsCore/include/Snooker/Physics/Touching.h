#pragma once
#include <Snooker/Physics/BallState.h>

namespace Snooker
{
enum class TouchingRelation { Separated, Touching, Overlapping };
enum class RelativeNormalMotion { Approaching, Resting, Separating };
struct TouchingThresholds
{
    Meters GapTolerance=1e-7;
    Scalar NormalSpeedTolerance=1e-7;
};
struct TouchingResult
{
    TouchingRelation Relation=TouchingRelation::Separated;
    RelativeNormalMotion Motion=RelativeNormalMotion::Resting;
    Meters SignedGap=0;
    Scalar RelativeNormalSpeed=0;
    Vec3d Normal;
};
bool QueryTouching(const BallState& First,const BallState& Second,const TouchingThresholds& Thresholds,
                   TouchingResult& Output,std::string* Error=nullptr);
}
