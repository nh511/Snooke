#pragma once
#include <Snooker/Rules/MultiBallShot.h>
#include <Snooker/Rules/ShotScoring.h>

namespace Snooker
{
struct TurnInput
{
    bool Foul=false;
    bool ContinueBreak=false;
    bool CueBallInHand=false;
    bool ShotCompleted=true;
    std::string Reason;
    bool IsValid(std::string* Error=nullptr) const;
};
struct TurnResult
{
    MatchState State;
    bool Switched=false;
    std::uint32_t PreviousPlayer=0,NextPlayer=0;
    bool ContinueBreak=false;
    bool IsValid(std::string* Error=nullptr) const;
};
bool ApplyTurnResult(const MatchState&,const TurnInput&,TurnResult& Output,std::string* Error=nullptr);
bool ApplyTurnResult(const MatchState&,const ShotScoreResult&,const MultiBallShotResult&,TurnResult& Output,std::string* Error=nullptr);
}
