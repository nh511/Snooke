#pragma once
#include <Snooker/Table/TableGeometry.h>
#include <Snooker/Physics/BallState.h>

namespace Snooker
{
enum class PocketFeature { Jaw, Rim, Slope, Bottom };
struct PocketContact
{
    bool Hit=false;
    bool InitiallyOverlapping=false;
    Seconds Time=0;
    std::uint32_t PocketId=0;
    PocketFeature Feature=PocketFeature::Jaw;
    std::size_t Primitive=0;
    Vec3d Point, Normal;
};
// Analytic, constant-velocity sphere sweep. Result is earliest contact in [0,Horizon].
// No hit is a valid query; malformed/unsafe inputs return false without changing output.
bool FindPocketContact(const BallState& Ball,const PocketGeometry& Pocket,Seconds Horizon,
                       PocketContact& Output,std::string* Error=nullptr);
// Frictionless local rebound at verified current contact. No pot/scoring side effects.
bool ReflectPocketContact(BallState& Ball,const PocketGeometry& Pocket,const PocketContact& Contact,
                           std::string* Error=nullptr);
}
