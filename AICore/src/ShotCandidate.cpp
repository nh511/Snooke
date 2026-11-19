#include <Snooker/AI/ShotCandidate.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string*e,const char*m){if(e)*e=m;return false;}const BallDefinition* Def(const IAIStateQuery&q,BallId id){auto i=std::find_if(q.Definitions().begin(),q.Definitions().end(),[&](const auto&d){return d.Id==id;});return i==q.Definitions().end()?nullptr:&*i;}}
bool ShotCandidate::IsValid(std::string* Error)const
{
    const double length=CueDirection.Length();if(!CueBall||!TargetBall||CueBall==TargetBall||!Pocket||!CueDirection.IsFinite()||!GhostCenter.IsFinite()||!StrikePointOnBall.IsFinite()||!CueTipOffsetMeters.IsFinite()||!NearlyEqual(length,1,1e-9)||!IsFinite(ForwardSpeedMetersPerSecond)||ForwardSpeedMetersPerSecond<=0||!IsFinite(CutAngleRadians)||CutAngleRadians<0||CutAngleRadians>Pi||!IsFinite(Risk)||Risk<0)return Fail(Error,"invalid AI shot candidate");return true;
}
bool ShotCandidate::ToShotInput(const IAIStateQuery&q,ShotInput&out,std::string* Error)const
{
    if(!IsValid(Error))return false;const auto* cue=q.FindBall(CueBall);if(!cue)return Fail(Error,"candidate cue absent");ShotInput input;input.CueBallId=CueBall;input.CueProfileId=q.Cue().Id;input.CueDirection=CueDirection;input.StrikePointOnBall=StrikePointOnBall;input.CueTipOffsetMeters=CueTipOffsetMeters;input.ForwardSpeedMetersPerSecond=ForwardSpeedMetersPerSecond;input.PullbackDistanceMeters=std::min(q.Cue().MaxPullbackMeters,ForwardSpeedMetersPerSecond*.04);if(!input.ValidateFor(q.Cue(),*cue,Error))return false;out=input;return true;
}
bool CreatePotCandidate(const IAIStateQuery&q,const TableGeometry&g,BallId targetId,std::uint32_t pocketId,Scalar speed,ShotCandidate&out,std::string* Error)
{
    const auto cueDef=std::find_if(q.Definitions().begin(),q.Definitions().end(),[](const auto&d){return d.Type==EBallType::Cue;});const auto* targetDef=Def(q,targetId);const auto* cue=cueDef==q.Definitions().end()?nullptr:q.FindBall(cueDef->Id);const auto* target=q.FindBall(targetId);const auto pocket=std::find_if(g.Pockets.begin(),g.Pockets.end(),[&](const auto&p){return p.Definition.Id==pocketId;});if(!cue||!target||!targetDef||pocket==g.Pockets.end()||!IsFinite(speed)||speed<=0)return Fail(Error,"unresolved candidate geometry/ball/speed");if(std::find(q.BallOn().LegalChoices.begin(),q.BallOn().LegalChoices.end(),targetDef->Type)==q.BallOn().LegalChoices.end())return Fail(Error,"target is not Ball On");
    auto toPocket=pocket->Definition.Center-target->PositionMeters;toPocket.Z=0;const double targetPath=toPocket.Length();if(targetPath<=targetDef->RadiusMeters)return Fail(Error,"target already at/behind pocket center");toPocket=toPocket*(1/targetPath);const auto ghost=target->PositionMeters-toPocket*(cueDef->RadiusMeters+targetDef->RadiusMeters);auto direction=ghost-cue->PositionMeters;direction.Z=0;const double cuePath=direction.Length();if(cuePath<=cueDef->RadiusMeters)return Fail(Error,"cue/ghost geometry degenerate");direction=direction*(1/cuePath);const double dot=std::clamp(direction.Dot(toPocket),-1.0,1.0);
    ShotCandidate result;result.CueBall=cue->Id;result.TargetBall=targetId;result.Pocket=pocketId;result.CueDirection=direction;result.GhostCenter=ghost;result.ForwardSpeedMetersPerSecond=speed;result.CutAngleRadians=std::acos(dot);result.Risk=result.CutAngleRadians/Pi+cuePath/(g.Cloth.Max.X-g.Cloth.Min.X)+targetPath/(g.Cloth.Max.Y-g.Cloth.Min.Y);if(!result.IsValid(Error))return false;ShotInput input;if(!result.ToShotInput(q,input,Error))return false;out=result;return true;
}
}
