#pragma once
#include <Snooker/Rules/FoulValue.h>

namespace Snooker
{
struct MultiBallShotResult
{
    bool Foul=false;
    bool CueBallPotted=false;
    bool NormalScoringAllowed=false;
    std::string Reason;
    std::vector<BallId> PottedOrder;
    std::vector<BallId> ScoringBalls;
    std::vector<BallId> IllegalBalls;
    bool IsValid(std::string* Error=nullptr) const;
};
bool EvaluateMultiBallShot(const MatchState&,const ShotPhysicsResult&,const FirstContactResult&,
                           const CueBallFoulResult&,MultiBallShotResult& Output,std::string* Error=nullptr);
}
