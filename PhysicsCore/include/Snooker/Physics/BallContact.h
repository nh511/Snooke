#pragma once
#include <Snooker/Physics/BallState.h>

namespace Snooker
{
struct BallContactTime
{
    bool Hit=false;
    bool InitiallyOverlapping=false;
    Seconds Time=0;
    Vec3d Normal; // From first center towards second center at contact.
};
// True means valid query, including no hit; false means invalid/numerically unsafe input.
// Output is unchanged on error. Translation is constant over [0,Horizon].
bool FindBallContactTime(const BallState& First,const BallState& Second,Seconds Horizon,
                          BallContactTime& Output,std::string* Error=nullptr);
}
