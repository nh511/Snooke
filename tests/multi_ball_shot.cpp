#include <Snooker/Rules/MultiBallShot.h>
#include <iostream>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL multi-ball "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
ShotPhysicsResult Fixture(FramePhase phase,std::vector<EBallType> types,bool cue=false)
{
    ShotPhysicsResult r;r.CueBall=1;r.FirstContact=2;r.CueBallPotted=cue;BallState a;a.Id=1;a.PositionMeters={0,0,a.RadiusMeters};
    PhysicsClock clock;std::vector<BallState> balls{a};BallDefinition cueDef;cueDef.Id=1;cueDef.Type=EBallType::Cue;cueDef.InitialPositionMeters=a.PositionMeters;r.Definitions={cueDef};
    for(std::size_t i=0;i<types.size();++i){auto b=a;b.Id=static_cast<BallId>(i+2);b.PositionMeters.X=.2+i*.1;balls.push_back(b);auto d=cueDef;d.Id=b.Id;d.Type=types[i];d.InitialPositionMeters=b.PositionMeters;r.Definitions.push_back(d);}
    r.Initial=PhysicsSnapshot::Capture(clock,balls,"multi-v1",ESnapshotPhase::BeforeShot);r.Final=r.Initial;r.Final.StepIndex=100;r.Final.SimulationTimeSeconds=100.0/240;r.Final.Phase=ESnapshotPhase::AfterShot;
    r.Events={{0,0,0,PhysicsEventKind::CueStrike,1,0,{},{},.1},{1,10,.01,PhysicsEventKind::BallBall,1,2,{},{},.1}};
    for(std::size_t i=0;i<types.size();++i){r.Potted.push_back(static_cast<BallId>(i+2));r.Final.PocketStates[i+1]={PocketState::Potted,1,true};r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),20+static_cast<StepIndex>(i),.1+i*.01,PhysicsEventKind::Potted,static_cast<BallId>(i+2),1,{},{},0});}
    if(cue){r.Potted.push_back(1);r.Final.PocketStates[0]={PocketState::Potted,1,true};r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),80,.12,PhysicsEventKind::Potted,1,1,{},{},0});}
    r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),100,r.Final.SimulationTimeSeconds,PhysicsEventKind::ShotSettled,0,0,{},{},0});r.Final.EventCount=r.Events.size();return r;
}
int main()
{
    std::string error;FirstContactResult first{FirstContactKind::LegalBallOn,1,2,EBallType::Red,EBallType::Red};CueBallFoulResult cue{false,false,false,1,{},{}};MatchState state;MultiBallShotResult result;
    const std::vector<BallId> two{2,3},oneId{2},threeId{3};
    auto reds=Fixture(FramePhase::Reds,{EBallType::Red,EBallType::Red});CHECK(EvaluateMultiBallShot(state,reds,first,cue,result,&error));CHECK(!result.Foul&&result.NormalScoringAllowed&&result.ScoringBalls==two);
    auto mixed=Fixture(FramePhase::Reds,{EBallType::Red,EBallType::Black});CHECK(EvaluateMultiBallShot(state,mixed,first,cue,result,&error));CHECK(result.Foul&&result.ScoringBalls.empty()&&result.IllegalBalls==threeId);
    state.Phase=FramePhase::ColorAfterRed;state.BallOn=EBallType::Blue;auto colors=Fixture(FramePhase::ColorAfterRed,{EBallType::Blue,EBallType::Blue});CHECK(EvaluateMultiBallShot(state,colors,first,cue,result,&error));CHECK(result.Foul&&result.Reason=="multiple colors potted");
    auto one=Fixture(FramePhase::ColorAfterRed,{EBallType::Blue});CHECK(EvaluateMultiBallShot(state,one,first,cue,result,&error));CHECK(!result.Foul&&result.ScoringBalls==oneId);
    auto cueShot=Fixture(FramePhase::Reds,{EBallType::Red},true);auto cueFoul=CueBallFoulResult{true,true,true,1,{2},"cue ball potted"};CHECK(EvaluateMultiBallShot(state,cueShot,first,cueFoul,result,&error));CHECK(result.Foul&&!result.NormalScoringAllowed);
    first.Kind=FirstContactKind::WrongBall;CHECK(EvaluateMultiBallShot(state,one,first,cue,result,&error));CHECK(result.Foul&&result.ScoringBalls.empty());
    std::cout<<"PASS multi-red normal, illegal mixed pot, multiple colors, cue pot and wrong-contact classification\n";
}
