#pragma once
#include <Snooker/AI/ShotEvaluation.h>

namespace Snooker
{
struct BasicAIResult {ShotCandidate Candidate;ShotEvaluation Evaluation;std::size_t Generated=0,Evaluated=0;std::string Detail;bool IsValid(std::string* Error=nullptr) const;};
bool ChooseBasicScoringShot(const IAIStateQuery&,const PhysicsBundle&,const TableGeometry&,BasicAIResult& Output,std::string* Error=nullptr);
}
