#include <Snooker/Rules/FinalColors.h>
#include <iostream>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL final colors "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
ShotPhysicsResult Pot(EBallType type,bool potted=true)
{
 ShotPhysicsResult r;r.CueBall=1;r.FirstContact=2;BallState a;a.Id=1;a.PositionMeters={0,0,a.RadiusMeters};auto b=a;b.Id=2;b.PositionMeters.X=.2;PhysicsClock c;r.Initial=PhysicsSnapshot::Capture(c,{a,b},"final-v1",ESnapshotPhase::BeforeShot);r.Final=r.Initial;r.Final.StepIndex=10;r.Final.SimulationTimeSeconds=10.0/240;r.Final.Phase=ESnapshotPhase::AfterShot;BallDefinition x;x.Id=1;x.Type=EBallType::Cue;x.InitialPositionMeters=a.PositionMeters;auto y=x;y.Id=2;y.Type=type;y.InitialPositionMeters=b.PositionMeters;r.Definitions={x,y};r.Events={{0,0,0,PhysicsEventKind::CueStrike,1,0,{},{},.1},{1,1,.001,PhysicsEventKind::BallBall,1,2,{},{},.1}};if(potted){r.Potted={2};r.Final.PocketStates[1]={PocketState::Potted,1,true};r.Events.push_back({2,5,.01,PhysicsEventKind::Potted,2,1,{},{},0});}r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),10,r.Final.SimulationTimeSeconds,PhysicsEventKind::ShotSettled,0,0,{},{},0});r.Final.EventCount=r.Events.size();return r;
}
int main(){std::string error;MatchState state;state.Phase=FramePhase::FinalColors;state.RemainingReds=0;state.BallOn=EBallType::Yellow;FinalColorResult result;std::uint32_t total=0;
 for(auto type:{EBallType::Yellow,EBallType::Green,EBallType::Brown,EBallType::Blue,EBallType::Pink,EBallType::Black}){CHECK(EvaluateFinalColor(state,Pot(type),result,&error));CHECK(result.Legal&&result.ColorRemoved&&result.State.Scores[0]>total);total=result.State.Scores[0];state=result.State;if(type!=EBallType::Black)CHECK(state.BallOn!=type);}
 CHECK(result.FrameColorsComplete&&state.Phase==FramePhase::Finished&&total==27);
 state=MatchState{};state.Phase=FramePhase::FinalColors;state.RemainingReds=0;state.BallOn=EBallType::Yellow;CHECK(EvaluateFinalColor(state,Pot(EBallType::Green),result,&error));CHECK(!result.Legal&&result.Reason=="wrong final color contact"&&result.State.Scores[0]==0);
 CHECK(EvaluateFinalColor(state,Pot(EBallType::Yellow,false),result,&error));CHECK(!result.Legal&&result.Reason=="final color not potted");
 auto saved=result.State.Serialize();state.Phase=FramePhase::Reds;state.RemainingReds=1;state.BallOn=EBallType::Red;CHECK(!EvaluateFinalColor(state,Pot(EBallType::Red),result,&error));CHECK(result.State.Serialize()==saved);
 std::cout<<"PASS yellow-to-black order, no respot, wrong sequence, miss and completed colors\n";}
