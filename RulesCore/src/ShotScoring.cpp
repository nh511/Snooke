#include <Snooker/Rules/ShotScoring.h>
#include <Snooker/Rules/BallValues.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool NominateColor(const MatchState& state,EBallType color,MatchState& output,std::string* error)
{
    if(!state.IsValid(error))return false;int value=0;
    if(state.Phase!=FramePhase::ColorAfterRed || state.ShotInProgress || !TryBallValue(color,value) || value<2)
        return Fail(error,"color nomination unavailable or invalid");
    auto next=state;next.BallOn=color;next.NominationPending=false;output=next;return true;
}
bool EvaluateShot(const MatchState& state,const ShotPhysicsResult& input,ShotScoreResult& output,std::string* error)
{
    if(!state.IsValid(error) || !input.IsValid(error))return false;
    if(state.Phase==FramePhase::Finished || state.ShotInProgress || state.MissPending || state.FreeBallAvailable || state.CueBallInHand)
        return Fail(error,"shot cannot be evaluated in current match state");
    if(state.NominationPending)return Fail(error,"nominate a color before evaluating the shot");
    if(state.Phase==FramePhase::FinalColors)return Fail(error,"final-color scoring pending RUL-012");
    const auto type=[&](BallId id){return std::find_if(input.Definitions.begin(),input.Definitions.end(),[&](const auto& d){return d.Id==id;})->Type;};
    if(input.CueBallPotted)return Fail(error,"cue-ball foul pending RUL-006");
    if(!input.FirstContact || type(input.FirstContact)!=state.BallOn)return Fail(error,"wrong/no first contact pending foul processing");
    unsigned reds=0;
    for(std::size_t i=0;i<input.Initial.Balls.size();++i) {
        const auto& b=input.Initial.Balls[i];
        const bool potted=!input.Initial.PocketStates.empty() && input.Initial.PocketStates[i].State==PocketState::Potted;
        if(!potted && type(b.Id)==EBallType::Red)++reds;
    }
    if(reds!=state.RemainingReds)return Fail(error,"match red count differs from physical state");
    ShotScoreResult result;result.State=state;
    for(auto id:input.Potted) {
        if(type(id)!=state.BallOn)return Fail(error,"illegal potted target pending foul processing");
        int value=0;if(!TryBallValue(type(id),value))return Fail(error,"invalid scoring ball");
        result.Points+=static_cast<unsigned>(value);
        if(type(id)!=EBallType::Red)result.ColorsToRespot.push_back(id);
    }
    if(state.Phase==FramePhase::ColorAfterRed && input.Potted.size()>1)return Fail(error,"multiple nominated colors are not a legal normal pot");
    if(result.Points>std::numeric_limits<std::uint32_t>::max()-result.State.Scores[state.CurrentPlayer])return Fail(error,"score overflow");
    result.State.Scores[state.CurrentPlayer]+=result.Points;result.ContinueBreak=result.Points>0;
    if(state.Phase==FramePhase::Reds && result.Points) {
        result.State.RemainingReds-=result.Points;result.State.Phase=FramePhase::ColorAfterRed;
        result.State.BallOn=EBallType::Yellow;result.State.NominationPending=true;
    } else if(state.Phase==FramePhase::ColorAfterRed) {
        result.State.Phase=state.RemainingReds?FramePhase::Reds:FramePhase::FinalColors;
        result.State.BallOn=state.RemainingReds?EBallType::Red:EBallType::Yellow;
    }
    result.State.ShotInProgress=false;
    if(!result.State.IsValid(error))return false;output=std::move(result);return true;
}
}
