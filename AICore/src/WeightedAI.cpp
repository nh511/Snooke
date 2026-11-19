#include <Snooker/AI/WeightedAI.h>
#include <algorithm>
#include <random>

namespace Snooker
{
namespace
{
bool Fail(std::string* error,const char* message){if(error)*error=message;return false;}
Scalar Safety(const ShotEvaluation& evaluation)
{
    const auto cue=std::find_if(evaluation.Record.Final.Balls.begin(),evaluation.Record.Final.Balls.end(),[&](const auto& ball){return ball.Id==evaluation.Record.Input.CueBallId;});
    if(cue==evaluation.Record.Final.Balls.end())return 0;
    Scalar nearest=10;
    for(std::size_t i=0;i<evaluation.Record.Final.Balls.size();++i) {
        const auto& ball=evaluation.Record.Final.Balls[i];
        if(ball.Id==cue->Id||(!evaluation.Record.Final.PocketStates.empty()&&evaluation.Record.Final.PocketStates[i].State==PocketState::Potted))continue;
        nearest=std::min(nearest,(ball.PositionMeters-cue->PositionMeters).Length());
    }
    return std::min(nearest,2.0);
}
}
bool DifficultyProfile::IsValid(std::string* error)const
{
    for(const auto value:{PotWeight,SafetyWeight,RiskWeight,ErrorRadians,ErrorWeight})if(!IsFinite(value)||value<0)return Fail(error,"invalid AI difficulty profile");
    return true;
}
DifficultyProfile DifficultyPreset(AIDifficulty difficulty,std::uint32_t seed)
{
    DifficultyProfile profile;profile.Difficulty=difficulty;profile.Seed=seed;
    if(difficulty==AIDifficulty::Easy){profile.PotWeight=1;profile.SafetyWeight=12;profile.RiskWeight=6;profile.ErrorRadians=.05;profile.ErrorWeight=30;}
    else if(difficulty==AIDifficulty::Hard){profile.PotWeight=12;profile.SafetyWeight=.5;profile.RiskWeight=.5;profile.ErrorRadians=0;profile.ErrorWeight=0;}
    return profile;
}
bool WeightedAIResult::IsValid(std::string* error)const
{
    if(!Evaluation.IsValid(error)||!IsFinite(Total)||!IsFinite(Offense)||!IsFinite(Safety)||!IsFinite(RiskPenalty)||!IsFinite(ErrorPenalty)||Detail.empty())return Fail(error,"invalid weighted AI result");
    return true;
}
bool SelectWeightedShot(const std::vector<ShotEvaluation>& evaluations,const DifficultyProfile& profile,WeightedAIResult& output,std::string* errorText)
{
    if(evaluations.empty()||!profile.IsValid(errorText))return Fail(errorText,"no evaluations or invalid profile");
    std::mt19937 rng(profile.Seed);std::normal_distribution<double> error(0,profile.ErrorRadians>0?profile.ErrorRadians:1);
    bool found=false;WeightedAIResult best;
    for(std::size_t i=0;i<evaluations.size();++i) {
        const auto& evaluation=evaluations[i];if(!evaluation.IsValid(errorText))return false;
        WeightedAIResult result;result.Index=i;result.Evaluation=evaluation;
        result.Offense=10*evaluation.LegalPotted-20*evaluation.CueBallPotted-4*(evaluation.TotalPotted-evaluation.LegalPotted-(evaluation.CueBallPotted?1:0));
        result.Safety=Safety(evaluation);result.RiskPenalty=evaluation.Candidate.Risk;
        result.ErrorPenalty=profile.ErrorRadians>0?std::abs(error(rng)):0;
        result.Total=profile.PotWeight*result.Offense+profile.SafetyWeight*result.Safety-profile.RiskWeight*result.RiskPenalty-profile.ErrorWeight*result.ErrorPenalty;
        result.Detail="difficulty="+std::to_string(unsigned(profile.Difficulty))+" offense="+std::to_string(result.Offense)+" safety="+std::to_string(result.Safety)+" risk="+std::to_string(result.RiskPenalty)+" error="+std::to_string(result.ErrorPenalty)+" total="+std::to_string(result.Total);
        if(!found||result.Total>best.Total+1e-12||(NearlyEqual(result.Total,best.Total,1e-12)&&std::tie(evaluation.Candidate.TargetBall,evaluation.Candidate.Pocket,evaluation.Candidate.ForwardSpeedMetersPerSecond)<std::tie(best.Evaluation.Candidate.TargetBall,best.Evaluation.Candidate.Pocket,best.Evaluation.Candidate.ForwardSpeedMetersPerSecond))){best=std::move(result);found=true;}
    }
    if(!found||!best.IsValid(errorText))return false;output=std::move(best);return true;
}
}
