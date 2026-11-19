#pragma once
#include <Snooker/Rules/ShotPhysicsResult.h>
#include <Snooker/Rules/MatchState.h>

namespace Snooker
{
struct FinalColorResult
{
    MatchState State;
    bool Legal=false,ColorRemoved=false,FrameColorsComplete=false;
    std::uint32_t Points=0;
    EBallType Expected=EBallType::Yellow,Actual=EBallType::Yellow;
    std::string Reason;
    bool IsValid(std::string* Error=nullptr) const;
};
bool EvaluateFinalColor(const MatchState&,const ShotPhysicsResult&,FinalColorResult& Output,std::string* Error=nullptr);
}
