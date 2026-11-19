#include <Snooker/Rules/FirstContact.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool FirstContactResult::IsValid(std::string* Error) const
{
    if(CueBall==0 || (Kind==FirstContactKind::NoContact && ContactedBall!=0) ||
       (Kind!=FirstContactKind::NoContact && ContactedBall==0) || Expected==EBallType::Cue || Actual==EBallType::Cue)
        return Fail(Error,"invalid first-contact result");
    return true;
}
bool EvaluateFirstContact(const MatchState& state,const ShotPhysicsResult& input,FirstContactResult& Output,std::string* Error)
{
    if(!state.IsValid(Error) || !input.IsValid(Error))return false;
    if(state.ShotInProgress || state.Phase==FramePhase::Finished)return Fail(Error,"first contact requires a completed active-frame shot");
    std::vector<BallId> ids;ids.reserve(input.Definitions.size());
    for(const auto& d:input.Definitions)ids.push_back(d.Id);
    const auto find=[&](BallId id)->const BallDefinition* {auto it=std::find_if(input.Definitions.begin(),input.Definitions.end(),[&](const auto& d){return d.Id==id;});return it==input.Definitions.end()?nullptr:&*it;};
    FirstContactResult result;result.CueBall=input.CueBall;result.Expected=state.BallOn;
    if(!input.FirstContact) {result.Kind=FirstContactKind::NoContact;result.Actual=state.BallOn;}
    else {
        const auto* definition=find(input.FirstContact);if(!definition)return Fail(Error,"first contact references unknown ball");
        if(definition->Type==EBallType::Cue)return Fail(Error,"first contact cannot be cue ball");
        result.ContactedBall=input.FirstContact;result.Actual=definition->Type;
        result.Kind=definition->Type==state.BallOn?FirstContactKind::LegalBallOn:FirstContactKind::WrongBall;
    }
    if(!result.IsValid(Error))return false;Output=std::move(result);return true;
}
}
