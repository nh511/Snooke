#pragma once
#include <Snooker/AI/ShotEvaluation.h>

namespace Snooker
{
enum class AIDifficulty { Easy, Medium, Hard };
struct DifficultyProfile {AIDifficulty Difficulty=AIDifficulty::Medium;std::uint32_t Seed=1;Scalar PotWeight=4,SafetyWeight=3,RiskWeight=2,ErrorRadians=.01,ErrorWeight=10;bool IsValid(std::string* Error=nullptr) const;};
DifficultyProfile DifficultyPreset(AIDifficulty,std::uint32_t Seed=1);
struct WeightedAIResult {std::size_t Index=0;ShotEvaluation Evaluation;Scalar Total=0,Offense=0,Safety=0,RiskPenalty=0,ErrorPenalty=0;std::string Detail;bool IsValid(std::string* Error=nullptr) const;};
bool SelectWeightedShot(const std::vector<ShotEvaluation>&,const DifficultyProfile&,WeightedAIResult& Output,std::string* Error=nullptr);
}
