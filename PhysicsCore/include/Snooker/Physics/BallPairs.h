#pragma once
#include <Snooker/Physics/BallState.h>
#include <vector>

namespace Snooker
{
struct BallPair
{
    BallId First=0, Second=0;
    bool operator==(const BallPair& Other) const { return First==Other.First && Second==Other.Second; }
};
// Constant-velocity sweep for one substep; rebuild after impulses/acceleration changes.
bool FindBallPairCandidates(const std::vector<BallState>& Balls,Seconds Horizon,std::vector<BallPair>& Output,
                            std::string* Error=nullptr);
}
