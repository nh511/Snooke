#include <Snooker/Rules/FoulValue.h>
#include <Snooker/Rules/BallValues.h>
#include <algorithm>
namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool FoulValueResult::IsValid(std::string* Error) const
{
    if(!Foul&&(Points||RelevantValue||!Explanation.empty()))return Fail(Error,"non-foul has foul fields");
    if(Foul&&(Points<4||AwardedToPlayer>1||RelevantValue<0||Explanation.empty()))return Fail(Error,"invalid foul value result");return true;
}
bool CalculateFoulValue(const MatchState& state,const ShotPhysicsResult& input,const FirstContactResult& contact,const CueBallFoulResult& cueFoul,FoulValueResult& Output,std::string* Error)
{
    if(!state.IsValid(Error)||!input.IsValid(Error)||!contact.IsValid(Error)||!cueFoul.IsValid(Error))return false;
    if(state.FreeBallAvailable||state.FreeBallNominated)return Fail(Error,"Free Ball foul value pending RUL-013");
    if(cueFoul.CueBall!=input.CueBall)return Fail(Error,"cue foul/input mismatch");
    FoulValueResult result;result.AwardedToPlayer=1-state.CurrentPlayer;std::vector<int> values;
    auto type=[&](BallId id)->EBallType{auto it=std::find_if(input.Definitions.begin(),input.Definitions.end(),[&](const auto& d){return d.Id==id;});return it==input.Definitions.end()?EBallType::Cue:it->Type;};
    if(cueFoul.Foul)values.push_back(4);
    if(contact.Kind==FirstContactKind::NoContact)values.push_back(0);
    if(contact.Kind==FirstContactKind::WrongBall){int v=0;TryBallValue(contact.Actual,v);values.push_back(v);}
    for(auto id:input.Potted)if(id!=input.CueBall&&type(id)!=state.BallOn){int v=0;TryBallValue(type(id),v);values.push_back(v);}
    if(values.empty()){if(!result.IsValid(Error))return false;Output=result;return true;}
    result.Foul=true;result.Reason=cueFoul.Foul?FoulReason::CueBallPotted:(contact.Kind==FirstContactKind::NoContact?FoulReason::NoFirstContact:(contact.Kind==FirstContactKind::WrongBall?FoulReason::WrongFirstContact:FoulReason::IllegalBallPotted));
    result.RelevantValue=*std::max_element(values.begin(),values.end());result.Points=static_cast<std::uint32_t>(std::max(4,result.RelevantValue));
    switch(result.Reason){case FoulReason::CueBallPotted:result.Explanation="cue ball potted";break;case FoulReason::NoFirstContact:result.Explanation="no legal first contact";break;case FoulReason::WrongFirstContact:result.Explanation="wrong first contact";break;case FoulReason::IllegalBallPotted:result.Explanation="illegal ball potted";break;default:result.Explanation="multiple foul reasons";break;}
    if(!result.IsValid(Error))return false;Output=result;return true;
}
}
