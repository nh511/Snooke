#include <Snooker/AI/ShotEvaluation.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string*e,const char*m){if(e)*e=m;return false;}}
bool ShotEvaluation::IsValid(std::string* Error)const{if(!Candidate.IsValid(Error)||!Record.IsValid(Error)||!IsFinite(Score)||LegalPotted>TotalPotted||Detail.empty())return Fail(Error,"invalid AI shot evaluation");return true;}
bool EvaluateCandidate(const IAIStateQuery&q,const PhysicsBundle&bundle,const ShotCandidate&candidate,ShotEvaluation&Output,std::string*Error)
{
    if(bundle.Version!=q.Physics().PhysicsParameterVersion)return Fail(Error,"AI bundle/query version mismatch");const auto hash=q.StateHash();ShotInput input;if(!candidate.ToShotInput(q,input,Error))return false;auto service=CreatePhysicsService(bundle,Error);if(!service||!service->BeginShot(q.Physics(),input,Error))return false;PhysicsRunState state=PhysicsRunState::Running;for(unsigned n=0;n<100000&&state==PhysicsRunState::Running;++n)if(!service->Step(1.0/60,state,Error))return false;if(state!=PhysicsRunState::Settled)return Fail(Error,"AI candidate faulted or did not settle");ShotEvaluation result;result.Candidate=candidate;result.Record.Parameters=bundle;result.Record.Initial=q.Physics();result.Record.Input=input;result.Record.Final=service->SaveSnapshot();result.Record.Events=service->GetEvents();if(!result.Record.IsValid(Error))return false;
    for(const auto&e:result.Record.Events)if(e.Kind==PhysicsEventKind::Potted){++result.TotalPotted;if(e.Ball==input.CueBallId)result.CueBallPotted=true;else{auto d=std::find_if(q.Definitions().begin(),q.Definitions().end(),[&](const auto&x){return x.Id==e.Ball;});if(d!=q.Definitions().end()&&std::find(q.BallOn().LegalChoices.begin(),q.BallOn().LegalChoices.end(),d->Type)!=q.BallOn().LegalChoices.end())++result.LegalPotted;}}
    result.Score=10*result.LegalPotted-20*result.CueBallPotted-3*(result.TotalPotted-result.LegalPotted-(result.CueBallPotted?1:0))-candidate.Risk;result.Detail="legal="+std::to_string(result.LegalPotted)+" total="+std::to_string(result.TotalPotted)+" cue="+std::to_string(result.CueBallPotted)+" risk="+std::to_string(candidate.Risk);PhysicsShotRecord replay;if(!ReplayShot(result.Record,replay,Error)||replay.Serialize()!=result.Record.Serialize())return Fail(Error,"AI evaluation replay mismatch");if(q.StateHash()!=hash)return Fail(Error,"AI query mutated during evaluation");if(!result.IsValid(Error))return false;Output=std::move(result);return true;
}
}
