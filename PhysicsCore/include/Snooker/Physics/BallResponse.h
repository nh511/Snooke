#pragma once
#include <Snooker/Physics/BallState.h>

namespace Snooker
{
struct BallCollisionProfile
{
    std::string ParameterVersion;
    Scalar Restitution=0, TangentialFriction=0;
    Meters ContactTolerance=0;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,BallCollisionProfile& Output,std::string* Error=nullptr);
};
struct BallCollisionResponse
{
    bool Applied=false;
    Scalar NormalImpulse=0;
    Vec3d TangentialImpulse;
    Vec3d Point, Normal, RelativeContactVelocity;
    Scalar EnergyBefore=0, EnergyAfter=0;
};
Scalar BallKineticEnergy(const BallState& Ball);
// Call at the computed contact, not after free-stepping through it. Gross overlap is rejected.
bool ResolveBallContact(BallState& First,BallState& Second,const BallCollisionProfile& Profile,
                        BallCollisionResponse& Output,std::string* Error=nullptr);
}
