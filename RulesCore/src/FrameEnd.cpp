#include <Snooker/Rules/FrameEnd.h>
#include <limits>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool FrameEndResult::IsValid(std::string* Error) const
{
    if(!State.IsValid(Error)||Winner>1||FrameEnded==NeedsRespottedBlack)return Fail(Error,"invalid frame-end result flags");
    if(FrameEnded&&(State.Phase!=FramePhase::Finished||State.Scores[0]==State.Scores[1]||Winner!=(State.Scores[1]>State.Scores[0])))return Fail(Error,"invalid finished frame result");
    if(NeedsRespottedBlack&&(State.Phase!=FramePhase::FinalColors||!State.RespottedBlack||State.BallOn!=EBallType::Black||State.Scores[0]!=State.Scores[1]))return Fail(Error,"invalid respotted black result");
    return true;
}
bool ResolveFrameEnd(const MatchState& state,std::uint32_t starter,FrameEndResult& Output,std::string* Error)
{
    if(!state.IsValid(Error)||starter>1)return false;
    FrameEndResult result;result.State=state;
    if(state.Phase==FramePhase::Finished) {
        if(state.Scores[0]==state.Scores[1]) {
            result.State.Phase=FramePhase::FinalColors;result.State.BallOn=EBallType::Black;result.State.RespottedBlack=true;
            result.State.CurrentPlayer=starter;result.State.ShotInProgress=false;result.NeedsRespottedBlack=true;
        } else {result.FrameEnded=true;result.Winner=state.Scores[1]>state.Scores[0];}
    } else if(state.Phase==FramePhase::FinalColors&&state.RespottedBlack&&state.Scores[0]!=state.Scores[1]) {
        result.State.Phase=FramePhase::Finished;result.State.RespottedBlack=false;result.State.ShotInProgress=false;
        result.State.MissPending=false;result.State.FreeBallAvailable=false;result.State.CueBallInHand=false;
        result.FrameEnded=true;result.Winner=state.Scores[1]>state.Scores[0];
    } else return Fail(Error,"frame is not ready to end");
    if(!result.IsValid(Error))return false;Output=result;return true;
}
bool StartNextFrame(const MatchState& finished,const TableDefinition& table,const BallDefinition& prototype,
                    const OpeningLayout& layout,const std::string& version,Seconds step,std::uint32_t breaker,
                    FrameOpening& Output,std::string* Error)
{
    if(!finished.IsValid(Error)||finished.Phase!=FramePhase::Finished||finished.Scores[0]==finished.Scores[1]||finished.FrameNumber==std::numeric_limits<std::uint32_t>::max())
        return Fail(Error,"next frame requires a completed non-tied frame");
    return CreateFrameOpening(table,prototype,layout,version,step,finished.FrameNumber+1,breaker,Output,Error);
}
}
