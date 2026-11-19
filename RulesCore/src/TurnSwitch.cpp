#include <Snooker/Rules/TurnSwitch.h>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool TurnInput::IsValid(std::string* Error) const
{
    if(!ShotCompleted || (Foul&&!(!Reason.empty())) || (!Foul&&CueBallInHand))return Fail(Error,"invalid turn input");
    return true;
}
bool TurnResult::IsValid(std::string* Error) const
{
    if(!State.IsValid(Error)||PreviousPlayer>1||NextPlayer>1||Switched!=(PreviousPlayer!=NextPlayer)||ContinueBreak!=(!Switched))
        return Fail(Error,"invalid turn result");
    return true;
}
bool ApplyTurnResult(const MatchState& state,const TurnInput& input,TurnResult& Output,std::string* Error)
{
    if(!state.IsValid(Error)||!input.IsValid(Error))return false;
    TurnResult result;result.State=state;result.PreviousPlayer=state.CurrentPlayer;result.ContinueBreak=input.ContinueBreak&&!input.Foul;
    const bool keep=result.ContinueBreak;
    if(!keep){result.State.CurrentPlayer=1-state.CurrentPlayer;result.Switched=true;}
    result.NextPlayer=result.State.CurrentPlayer;result.State.ShotInProgress=false;
    if(input.CueBallInHand)result.State.CueBallInHand=true;
    if(!result.IsValid(Error))return false;Output=result;return true;
}
bool ApplyTurnResult(const MatchState& state,const ShotScoreResult& score,const MultiBallShotResult& shot,TurnResult& Output,std::string* Error)
{
    if(!state.IsValid(Error)||!score.State.IsValid(Error)||!shot.IsValid(Error)||score.State.CurrentPlayer!=state.CurrentPlayer)
        return false;
    TurnInput input;input.Foul=shot.Foul;input.ContinueBreak=!shot.Foul&&shot.NormalScoringAllowed&&score.ContinueBreak;
    input.CueBallInHand=shot.CueBallPotted;input.Reason=shot.Reason;return ApplyTurnResult(state,input,Output,Error);
}
}
