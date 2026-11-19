#pragma once
#include <Snooker/AI/ShotCandidate.h>
#include <Snooker/Replay/ShotReplay.h>

namespace Snooker
{
struct ShotEvaluation
{
    ShotCandidate Candidate;PhysicsShotRecord Record;
    std::uint32_t LegalPotted=0,TotalPotted=0;bool CueBallPotted=false;
    Scalar Score=0;std::string Detail;
    bool IsValid(std::string* Error=nullptr) const;
};
bool EvaluateCandidate(const IAIStateQuery&,const PhysicsBundle&,const ShotCandidate&,
                       ShotEvaluation& Output,std::string* Error=nullptr);
}
