#pragma once
#include <Snooker/Data/ClothProfile.h>
#include <Snooker/Physics/BallState.h>

namespace Snooker
{
struct ClothEnvironment
{
    Scalar GravityMetersPerSecondSquared=9.80665;
};
// Velocity-only contact solver for a homogeneous solid sphere on a flat XY cloth.
// Position integration, rolling resistance and sleep are separate tasks.
bool ApplySlidingFriction(BallState& Ball, const ClothProfile& Cloth, const ClothEnvironment& Environment,
                          Seconds Delta, std::string* Error=nullptr);
bool ApplyRollingResistance(BallState& Ball, const ClothProfile& Cloth, const ClothEnvironment& Environment,
                            Seconds Delta, std::string* Error=nullptr);
// Calls sliding contact integration once, then damps only vertical side spin.
// Do not call ApplySlidingFriction separately for the same substep.
bool ApplyAngularUpdate(BallState& Ball, const ClothProfile& Cloth, const ClothEnvironment& Environment,
                        Seconds Delta, std::string* Error=nullptr);
}
