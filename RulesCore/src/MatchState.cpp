#include <Snooker/Rules/MatchState.h>
#include <Snooker/Serialization/Text.h>
#include <locale>
#include <sstream>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) {if(Error)*Error=Message;return false;}
bool ReadBool(std::istream& s,bool& result)
{
    std::uint32_t v=0;if(!ReadUnsigned(s,v) || v>1)return false;result=v!=0;return true;
}
bool ParsePhase(const std::string& name,FramePhase& phase)
{
    for(auto p:{FramePhase::Reds,FramePhase::ColorAfterRed,FramePhase::FinalColors,FramePhase::Finished})
        if(name==FramePhaseName(p)){phase=p;return true;}
    return false;
}
}
const char* FramePhaseName(FramePhase Phase)
{
    switch(Phase) {
    case FramePhase::Reds:return "Reds";
    case FramePhase::ColorAfterRed:return "ColorAfterRed";
    case FramePhase::FinalColors:return "FinalColors";
    case FramePhase::Finished:return "Finished";
    }
    return "Unknown";
}
bool MatchState::IsValid(std::string* Error) const
{
    if(FrameNumber==0 || CurrentPlayer>1 || RemainingReds>15 ||
       std::string(FramePhaseName(Phase))=="Unknown" || std::string(BallTypeToString(BallOn))=="Unknown" || BallOn==EBallType::Cue)
        return Fail(Error,"invalid frame, player, phase, ball-on or red count");
    if((Phase==FramePhase::Reds && (RemainingReds==0 || BallOn!=EBallType::Red)) ||
       (Phase==FramePhase::ColorAfterRed && BallOn==EBallType::Red) ||
       (Phase==FramePhase::FinalColors && (RemainingReds!=0 || BallOn==EBallType::Red)) ||
       (Phase==FramePhase::Finished && (RemainingReds!=0 || ShotInProgress || MissPending || FreeBallAvailable || CueBallInHand)))
        return Fail(Error,"inconsistent frame phase state");
    if((NominationPending && Phase!=FramePhase::ColorAfterRed) ||
       (FreeBallNominated && (!FreeBallAvailable || FreeBallType==EBallType::Cue ||
         std::string(BallTypeToString(FreeBallType))=="Unknown")) ||
       (RespottedBlack && (RemainingReds!=0 || BallOn!=EBallType::Black || Phase!=FramePhase::FinalColors)) ||
       (Phase==FramePhase::Finished && (NominationPending || FreeBallNominated || RespottedBlack)))
        return Fail(Error,"inconsistent nomination/free-ball/black state");
    return true;
}
std::string MatchState::Serialize() const
{
    std::ostringstream s;s.imbue(std::locale::classic());
    s<<"snooker-match-v2 "<<FrameNumber<<' '<<FramePhaseName(Phase)<<' '<<CurrentPlayer<<' '
     <<Scores[0]<<' '<<Scores[1]<<' '<<BallTypeToString(BallOn)<<' '<<RemainingReds<<' '
     <<int(CueBallInHand)<<' '<<int(FreeBallAvailable)<<' '<<int(MissPending)<<' '<<int(ShotInProgress)<<' '
     <<int(NominationPending)<<' '<<int(FreeBallNominated)<<' '<<BallTypeToString(FreeBallType)<<' '<<int(RespottedBlack);
    return s.str();
}
bool MatchState::Deserialize(const std::string& Text,MatchState& Output,std::string* Error)
{
    std::istringstream s(Text);s.imbue(std::locale::classic());MatchState state;std::string magic,phase,on;
    if(!(s>>magic) || (magic!="snooker-match-v1" && magic!="snooker-match-v2") || !ReadUnsigned(s,state.FrameNumber) || !(s>>phase) ||
       !ParsePhase(phase,state.Phase) || !ReadUnsigned(s,state.CurrentPlayer) ||
       !ReadUnsigned(s,state.Scores[0]) || !ReadUnsigned(s,state.Scores[1]) || !(s>>on) ||
       !TryParseBallType(on,state.BallOn) || !ReadUnsigned(s,state.RemainingReds) ||
       !ReadBool(s,state.CueBallInHand) || !ReadBool(s,state.FreeBallAvailable) ||
       !ReadBool(s,state.MissPending) || !ReadBool(s,state.ShotInProgress))return Fail(Error,"invalid match schema or fields");
    if(magic=="snooker-match-v2") {
        std::string free;
        if(!ReadBool(s,state.NominationPending) || !ReadBool(s,state.FreeBallNominated) || !(s>>free) ||
           !TryParseBallType(free,state.FreeBallType) || !ReadBool(s,state.RespottedBlack))return Fail(Error,"invalid match v2 fields");
    }
    s>>std::ws;if(!s.eof())return Fail(Error,"unexpected match state tail");
    if(!state.IsValid(Error))return false;
    Output=state;return true;
}
}
