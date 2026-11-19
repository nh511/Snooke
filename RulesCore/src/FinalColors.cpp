#include <Snooker/Rules/FinalColors.h>
#include <Snooker/Rules/BallValues.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;} EBallType Next(EBallType t){return t==EBallType::Yellow?EBallType::Green:t==EBallType::Green?EBallType::Brown:t==EBallType::Brown?EBallType::Blue:t==EBallType::Blue?EBallType::Pink:EBallType::Black;}}
bool FinalColorResult::IsValid(std::string* Error) const
{
    if(!State.IsValid(Error)||Expected==EBallType::Cue||Expected==EBallType::Red||Actual==EBallType::Cue||Actual==EBallType::Red)return Fail(Error,"invalid final-color result");
    if(Legal&&(Points<2||!ColorRemoved||!Reason.empty()))return Fail(Error,"invalid legal final-color result");
    if(!Legal&&(Points||ColorRemoved||Reason.empty()))return Fail(Error,"invalid foul final-color result");
    return true;
}
bool EvaluateFinalColor(const MatchState& state,const ShotPhysicsResult& input,FinalColorResult& Output,std::string* Error)
{
    if(!state.IsValid(Error)||!input.IsValid(Error))return false;
    if(state.Phase!=FramePhase::FinalColors||state.RemainingReds!=0||state.ShotInProgress||state.FreeBallAvailable)return Fail(Error,"final-color evaluation unavailable");
    FinalColorResult result;result.State=state;result.Expected=state.BallOn;result.Actual=state.BallOn;
    const auto type=[&](BallId id){return std::find_if(input.Definitions.begin(),input.Definitions.end(),[&](const auto& d){return d.Id==id;})->Type;};
    if(!input.FirstContact){result.Reason="no first contact";if(!result.IsValid(Error))return false;Output=result;return true;}
    result.Actual=type(input.FirstContact);
    if(result.Actual!=state.BallOn){result.Reason="wrong final color contact";if(!result.IsValid(Error))return false;Output=result;return true;}
    if(input.CueBallPotted){result.Reason="cue ball potted";if(!result.IsValid(Error))return false;Output=result;return true;}
    if(input.Potted.size()!=1||type(input.Potted[0])!=state.BallOn){result.Reason=input.Potted.empty()?"final color not potted":"illegal final color pot";if(!result.IsValid(Error))return false;Output=result;return true;}
    int value=0;TryBallValue(state.BallOn,value);result.Legal=true;result.ColorRemoved=true;result.Points=static_cast<std::uint32_t>(value);
    if(result.Points>std::numeric_limits<std::uint32_t>::max()-result.State.Scores[state.CurrentPlayer])return Fail(Error,"final color score overflow");
    result.State.Scores[state.CurrentPlayer]+=result.Points;
    if(state.BallOn==EBallType::Black){result.FrameColorsComplete=true;result.State.Phase=FramePhase::Finished;result.State.ShotInProgress=false;result.State.MissPending=false;result.State.FreeBallAvailable=false;result.State.CueBallInHand=false;}
    else result.State.BallOn=Next(state.BallOn);
    if(!result.IsValid(Error))return false;Output=std::move(result);return true;
}
}
