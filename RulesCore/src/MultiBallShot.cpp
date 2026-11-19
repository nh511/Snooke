#include <Snooker/Rules/MultiBallShot.h>
#include <algorithm>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}}
bool MultiBallShotResult::IsValid(std::string* Error) const
{
    if(Foul && (NormalScoringAllowed || Reason.empty()))return Fail(Error,"foul result allows normal scoring");
    if(!Foul && !IllegalBalls.empty())return Fail(Error,"legal result contains illegal balls");
    for(std::size_t i=0;i<PottedOrder.size();++i)if(PottedOrder[i]==0||std::find(PottedOrder.begin(),PottedOrder.begin()+i,PottedOrder[i])!=PottedOrder.begin()+i)return Fail(Error,"duplicate potted order");
    return true;
}
bool EvaluateMultiBallShot(const MatchState& state,const ShotPhysicsResult& input,const FirstContactResult& first,
                           const CueBallFoulResult& cue,MultiBallShotResult& Output,std::string* Error)
{
    if(!state.IsValid(Error)||!input.IsValid(Error)||!first.IsValid(Error)||!cue.IsValid(Error))return false;
    MultiBallShotResult result;result.PottedOrder=input.Potted;result.CueBallPotted=input.CueBallPotted;
    auto type=[&](BallId id)->EBallType{auto it=std::find_if(input.Definitions.begin(),input.Definitions.end(),[&](const auto& d){return d.Id==id;});return it==input.Definitions.end()?EBallType::Cue:it->Type;};
    if(cue.Foul||input.CueBallPotted){result.Foul=true;result.Reason="cue ball potted";return result.IsValid(Error)?(Output=result,true):false;}
    if(first.Kind!=FirstContactKind::LegalBallOn){result.Foul=true;result.Reason=first.Kind==FirstContactKind::NoContact?"no legal first contact":"wrong first contact";return result.IsValid(Error)?(Output=result,true):false;}
    for(const auto id:input.Potted) {
        if(id==input.CueBall){result.Foul=true;result.Reason="cue ball potted";return result.IsValid(Error)?(Output=result,true):false;}
        const auto ballType=type(id);
        const bool legal=state.Phase==FramePhase::Reds?ballType==EBallType::Red:ballType==state.BallOn;
        if(legal)result.ScoringBalls.push_back(id);else result.IllegalBalls.push_back(id);
    }
    if(state.Phase==FramePhase::ColorAfterRed && result.ScoringBalls.size()>1) {
        result.Foul=true;result.Reason="multiple colors potted";result.IllegalBalls=result.ScoringBalls;result.ScoringBalls.clear();
    } else if(!result.IllegalBalls.empty()) {result.Foul=true;result.Reason="illegal ball potted";result.ScoringBalls.clear();}
    else {result.NormalScoringAllowed=true;}
    if(!result.IsValid(Error))return false;Output=std::move(result);return true;
}
}
