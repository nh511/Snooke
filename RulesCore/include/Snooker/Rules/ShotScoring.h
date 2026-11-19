#pragma once
#include <Snooker/Rules/ShotPhysicsResult.h>
#include <Snooker/Rules/MatchState.h>

namespace Snooker
{
struct ShotScoreResult
{
    MatchState State;
    std::uint32_t Points=0;
    bool ContinueBreak=false;
    std::vector<BallId> ColorsToRespot;
};
bool NominateColor(const MatchState&,EBallType,MatchState& Output,std::string* Error=nullptr);
// Initial normal-path scorer; foul and final-color processing are separate tasks.
bool EvaluateShot(const MatchState&,const ShotPhysicsResult&,ShotScoreResult& Output,std::string* Error=nullptr);
}
