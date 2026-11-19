#include <Snooker/Rules/TurnSwitch.h>
#include <iostream>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL turn "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
int main()
{
    std::string error;MatchState state;TurnResult result;TurnInput input;
    input.ContinueBreak=true;CHECK(ApplyTurnResult(state,input,result,&error));CHECK(!result.Switched&&result.NextPlayer==0&&result.State.ShotInProgress==false);
    input.ContinueBreak=false;CHECK(ApplyTurnResult(state,input,result,&error));CHECK(result.Switched&&result.PreviousPlayer==0&&result.NextPlayer==1);
    input.Foul=true;input.Reason="wrong first contact";input.ContinueBreak=true;input.CueBallInHand=true;CHECK(ApplyTurnResult(state,input,result,&error));CHECK(result.Switched&&result.State.CueBallInHand&&result.ContinueBreak==false);
    state.CurrentPlayer=1;input={};input.ContinueBreak=false;CHECK(ApplyTurnResult(state,input,result,&error));CHECK(result.PreviousPlayer==1&&result.NextPlayer==0);
    const auto saved=result.State.Serialize();input.Foul=true;input.Reason="";CHECK(!ApplyTurnResult(state,input,result,&error));CHECK(result.State.Serialize()==saved);
    input={};input.ShotCompleted=false;CHECK(!ApplyTurnResult(state,input,result,&error));CHECK(result.State.Serialize()==saved);
    MatchState scoreState;ShotScoreResult score;score.State=scoreState;score.Points=1;score.ContinueBreak=true;
    MultiBallShotResult shot;shot.NormalScoringAllowed=true;shot.ScoringBalls={2};CHECK(ApplyTurnResult(scoreState,score,shot,result,&error));CHECK(!result.Switched&&result.ContinueBreak);
    shot.Foul=true;shot.NormalScoringAllowed=false;shot.Reason="illegal ball potted";CHECK(ApplyTurnResult(scoreState,score,shot,result,&error));CHECK(result.Switched);
    std::cout<<"PASS continue-break, empty shot, foul/cue-in-hand turn switching and transactional rejection\n";
}
