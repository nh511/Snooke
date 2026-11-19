#include <Snooker/AI/BasicScoringAI.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string*e,const char*m){if(e)*e=m;return false;}}
bool BasicAIResult::IsValid(std::string* Error)const{if(!Candidate.IsValid(Error)||!Evaluation.IsValid(Error)||!Generated||!Evaluated||Evaluated>Generated||Detail.empty())return Fail(Error,"invalid basic AI result");return true;}
bool ChooseBasicScoringShot(const IAIStateQuery&q,const PhysicsBundle&bundle,const TableGeometry&g,BasicAIResult&Output,std::string*Error)
{
    const auto hash=q.StateHash();BasicAIResult result;bool found=false;std::string last;
    for(const auto& d:q.Definitions()) {
        if(std::find(q.BallOn().LegalChoices.begin(),q.BallOn().LegalChoices.end(),d.Type)==q.BallOn().LegalChoices.end())continue;
        const auto it=std::find_if(q.Physics().Balls.begin(),q.Physics().Balls.end(),[&](const auto& b){return b.Id==d.Id;});if(it==q.Physics().Balls.end())continue;const auto index=static_cast<std::size_t>(it-q.Physics().Balls.begin());if(!q.Physics().PocketStates.empty()&&q.Physics().PocketStates[index].State==PocketState::Potted)continue;
        for(const auto&p:g.Pockets)for(double speed:{.6,1.0,1.4}) {++result.Generated;ShotCandidate candidate;std::string error;if(!CreatePotCandidate(q,g,d.Id,p.Definition.Id,speed,candidate,&error)){last=error;continue;}ShotEvaluation evaluation;if(!EvaluateCandidate(q,bundle,candidate,evaluation,&error)){last=error;continue;}++result.Evaluated;const auto better=!found||evaluation.Score>result.Evaluation.Score+1e-12||(NearlyEqual(evaluation.Score,result.Evaluation.Score,1e-12)&&std::tie(candidate.Risk,candidate.TargetBall,candidate.Pocket,candidate.ForwardSpeedMetersPerSecond)<std::tie(result.Candidate.Risk,result.Candidate.TargetBall,result.Candidate.Pocket,result.Candidate.ForwardSpeedMetersPerSecond));if(better){found=true;result.Candidate=candidate;result.Evaluation=evaluation;}}
    }
    if(!found)return Fail(Error,last.empty()?"no legal AI candidates":"all AI candidates failed");
    result.Detail="generated="+std::to_string(result.Generated)+" evaluated="+std::to_string(result.Evaluated)+" selected="+std::to_string(result.Candidate.TargetBall)+" pocket="+std::to_string(result.Candidate.Pocket)+" "+result.Evaluation.Detail;
    if(q.StateHash()!=hash)return Fail(Error,"basic AI mutated query");if(!result.IsValid(Error))return false;Output=std::move(result);return true;
}
}
