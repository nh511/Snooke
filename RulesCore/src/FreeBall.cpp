#include <Snooker/Rules/FreeBall.h>
#include <Snooker/Rules/BallValues.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;} const BallDefinition* Find(const std::vector<BallDefinition>& d,BallId id){auto i=std::find_if(d.begin(),d.end(),[&](const auto& b){return b.Id==id;});return i==d.end()?nullptr:&*i;} bool Active(const PhysicsSnapshot& s,std::size_t i){return s.PocketStates.empty()||s.PocketStates[i].State!=PocketState::Potted;} bool Blocked(const Vec3d& a,const Vec3d& b,const PhysicsSnapshot& s,const std::vector<BallDefinition>& defs,BallId cue,BallId target,BallId excluded){const auto ab=b-a;const double length=ab.LengthSquared();for(std::size_t i=0;i<s.Balls.size();++i){const auto& ball=s.Balls[i];if(ball.Id==cue||ball.Id==target||ball.Id==excluded||!Active(s,i))continue;const auto* d=Find(defs,ball.Id);if(!d)continue;const double t=std::clamp((ball.PositionMeters-a).Dot(ab)/length,0.0,1.0);const auto nearest=a+ab*t;if((ball.PositionMeters-nearest).Length()<d->RadiusMeters+Find(defs,cue)->RadiusMeters-1e-10)return true;}return false;}}
bool SnookeredResult::IsValid(std::string* Error) const{if(LegalTargets.empty())return Fail(Error,"snookered query has no legal targets");return true;}
bool QuerySnookered(const PhysicsSnapshot& s,const std::vector<BallDefinition>& defs,BallId cue,EBallType on,SnookeredResult& Output,BallId excluded,std::string* Error)
{
    if(!s.IsValid(Error)||defs.size()!=s.Balls.size()||on==EBallType::Cue)return Fail(Error,"invalid snookered query");const auto* cueDef=Find(defs,cue);auto cueIt=std::find_if(s.Balls.begin(),s.Balls.end(),[&](const auto& b){return b.Id==cue;});if(!cueDef||cueIt==s.Balls.end())return Fail(Error,"missing cue ball");SnookeredResult result;bool anyClear=false;
    for(std::size_t i=0;i<s.Balls.size();++i){const auto* d=Find(defs,s.Balls[i].Id);if(!d||!d->IsValid(Error))return false;if(d->Type!=on||!Active(s,i))continue;result.LegalTargets.push_back(d->Id);const auto delta=s.Balls[i].PositionMeters-cueIt->PositionMeters;const double len=std::hypot(delta.X,delta.Y);if(len<=cueDef->RadiusMeters+d->RadiusMeters)continue;const Vec3d direction{delta.X/len,delta.Y/len,0},perp{-direction.Y,direction.X,0};const auto endpoint=s.Balls[i].PositionMeters-direction*(cueDef->RadiusMeters+d->RadiusMeters);const double offset=cueDef->RadiusMeters;const bool left=Blocked(cueIt->PositionMeters,endpoint+perp*offset,s,defs,cue,d->Id,excluded);const bool right=Blocked(cueIt->PositionMeters,endpoint-perp*offset,s,defs,cue,d->Id,excluded);if(!left||!right)anyClear=true;}
    if(result.LegalTargets.empty())return Fail(Error,"no active Ball On target");result.Snookered=!anyClear;if(!result.IsValid(Error))return false;Output=std::move(result);return true;
}
bool OfferFreeBall(const MatchState& state,const PhysicsSnapshot& s,const std::vector<BallDefinition>& defs,BallId cue,MatchState& Output,std::string* Error)
{
    if(!state.IsValid(Error)||state.Phase==FramePhase::Finished)return false;SnookeredResult result;if(!QuerySnookered(s,defs,cue,state.BallOn,result,0,Error))return false;auto next=state;next.FreeBallAvailable=result.Snookered;next.FreeBallNominated=false;if(!next.IsValid(Error))return false;Output=next;return true;
}
bool NominateFreeBall(const MatchState& state,EBallType type,MatchState& Output,std::string* Error)
{
    if(!state.IsValid(Error)||!state.FreeBallAvailable||state.ShotInProgress||type==EBallType::Cue||type==state.BallOn)return Fail(Error,"invalid Free Ball nomination");auto next=state;next.FreeBallNominated=true;next.FreeBallType=type;if(!next.IsValid(Error))return false;Output=next;return true;
}
bool FreeBallShotResult::IsValid(std::string* Error) const{if(!State.IsValid(Error)||(Foul&&(Points||Reason.empty()))||(!Foul&&!Reason.empty()))return Fail(Error,"invalid Free Ball result");return true;}
bool EvaluateFreeBallShot(const MatchState& state,const ShotPhysicsResult& input,FreeBallShotResult& Output,std::string* Error)
{
    if(!state.IsValid(Error)||!input.IsValid(Error)||!state.FreeBallAvailable||!state.FreeBallNominated)return Fail(Error,"Free Ball shot unavailable");auto type=[&](BallId id){return Find(input.Definitions,id)->Type;};FreeBallShotResult result;result.State=state;
    if(!input.FirstContact||type(input.FirstContact)!=state.FreeBallType){result.Foul=true;result.Reason="wrong Free Ball first contact";result.State.FreeBallAvailable=result.State.FreeBallNominated=false;if(!result.IsValid(Error))return false;Output=result;return true;}
    for(auto id:input.Potted)if(id!=input.CueBall&&type(id)!=state.FreeBallType){result.Foul=true;result.Reason="illegal ball potted with Free Ball";}
    if(input.CueBallPotted){result.Foul=true;result.Reason="cue ball potted";}
    if(!result.Foul&&!input.Potted.empty()){int value=0;TryBallValue(state.BallOn,value);result.Points=static_cast<std::uint32_t>(value);for(auto id:input.Potted)if(type(id)==state.FreeBallType)result.ColorsToRespot.push_back(id);}
    if(!result.Foul&&input.Potted.empty()){SnookeredResult withFree,withoutFree;if(!QuerySnookered(input.Final,input.Definitions,input.CueBall,state.BallOn,withFree,0,Error))return false;const auto nominated=std::find_if(input.Definitions.begin(),input.Definitions.end(),[&](const auto& d){return d.Type==state.FreeBallType&&d.Id!=input.CueBall;});if(nominated==input.Definitions.end())return Fail(Error,"nominated Free Ball absent");if(!QuerySnookered(input.Final,input.Definitions,input.CueBall,state.BallOn,withoutFree,nominated->Id,Error))return false;const bool exception=state.Phase==FramePhase::FinalColors&&state.BallOn==EBallType::Pink;if(withFree.Snookered&&!withoutFree.Snookered&&!exception){result.Foul=true;result.Reason="Free Ball created a snooker";}}
    result.State.FreeBallAvailable=false;result.State.FreeBallNominated=false;if(!result.IsValid(Error))return false;Output=std::move(result);return true;
}
}
