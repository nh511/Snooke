#include <Snooker/Rules/BallOn.h>
#include <utility>

namespace Snooker
{
bool GetBallOn(const MatchState& State,BallOnQuery& Output,std::string* Error)
{
    if(!State.IsValid(Error))return false;
    if(State.FreeBallAvailable){if(Error)*Error="Free Ball nomination is pending RUL-013";return false;}
    BallOnQuery query;query.Selected=State.BallOn;
    switch(State.Phase) {
    case FramePhase::Reds:query.LegalChoices={EBallType::Red};break;
    case FramePhase::ColorAfterRed:
        query.LegalChoices={EBallType::Yellow,EBallType::Green,EBallType::Brown,EBallType::Blue,EBallType::Pink,EBallType::Black};break;
    case FramePhase::FinalColors:query.LegalChoices={State.BallOn};break;
    case FramePhase::Finished:query.FrameFinished=true;break;
    }
    Output=std::move(query);return true;
}
}
