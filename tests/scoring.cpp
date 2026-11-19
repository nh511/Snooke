#include <Snooker/Rules/ShotScoring.h>
#include <iostream>
#include <limits>

using namespace Snooker;
#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL score "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
// Rules unit fixtures are explicit events, separate from real solver integration.
ShotPhysicsResult Pot(EBallType target,bool potted=true)
{
    ShotPhysicsResult r;r.CueBall=1;r.FirstContact=2;
    BallState cue;cue.Id=1;cue.PositionMeters={0,0,cue.RadiusMeters};
    auto ball=cue;ball.Id=2;ball.PositionMeters.X=0.2;
    PhysicsClock clock;r.Initial=PhysicsSnapshot::Capture(clock,{cue,ball},"rules-test",ESnapshotPhase::BeforeShot);
    r.Final=r.Initial;r.Final.StepIndex=240;r.Final.SimulationTimeSeconds=1;r.Final.Phase=ESnapshotPhase::AfterShot;
    BallDefinition a;a.Id=1;a.Type=EBallType::Cue;a.InitialPositionMeters=cue.PositionMeters;
    auto b=a;b.Id=2;b.Type=target;b.InitialPositionMeters=ball.PositionMeters;r.Definitions={a,b};
    r.Events={{0,0,0,PhysicsEventKind::CueStrike,1,0,{},TableNormal,0.1},
              {1,24,0.1,PhysicsEventKind::BallBall,1,2,{},TableNormal,0.1}};
    if(potted){r.Potted={2};r.Final.PocketStates[1]={PocketState::Potted,1,true};
        r.Events.push_back({2,120,0.5,PhysicsEventKind::Potted,2,1,{},TableNormal,0});}
    r.Events.push_back({r.Events.size(),240,1,PhysicsEventKind::ShotSettled,0,0,{},{},0});
    r.Final.EventCount=r.Events.size();return r;
}
int main()
{
    std::string error;MatchState state;state.RemainingReds=1;ShotScoreResult out;
    auto red=Pot(EBallType::Red);CHECK(red.IsValid(&error));CHECK(EvaluateShot(state,red,out,&error));
    CHECK(out.Points==1 && out.State.Scores[0]==1 && out.State.CurrentPlayer==0 && out.ContinueBreak);
    CHECK(out.State.RemainingReds==0 && out.State.Phase==FramePhase::ColorAfterRed && out.State.NominationPending);
    auto last=out.State;auto color=Pot(EBallType::Black);
    CHECK(!EvaluateShot(last,color,out,&error));CHECK(NominateColor(last,EBallType::Black,last,&error));
    CHECK(EvaluateShot(last,color,out,&error));CHECK(out.Points==7 && out.State.Scores[0]==8);
    CHECK(out.ColorsToRespot==std::vector<BallId>{2});CHECK(out.State.Phase==FramePhase::FinalColors && out.State.BallOn==EBallType::Yellow);
    unsigned expectedPoints=2;
    for(auto type:{EBallType::Yellow,EBallType::Green,EBallType::Brown,EBallType::Blue,EBallType::Pink,EBallType::Black}) {
        state=MatchState{};state.Phase=FramePhase::ColorAfterRed;state.RemainingReds=0;state.BallOn=EBallType::Yellow;
        state.NominationPending=true;CHECK(NominateColor(state,type,state,&error));
        CHECK(EvaluateShot(state,Pot(type),out,&error));CHECK(out.Points==expectedPoints++);
    }
    state=MatchState{};state.RemainingReds=1;
    CHECK(EvaluateShot(state,Pot(EBallType::Red,false),out,&error));CHECK(out.Points==0 && !out.ContinueBreak);
    const auto saved=out.State.Serialize();state.Scores[0]=std::numeric_limits<std::uint32_t>::max();
    CHECK(!EvaluateShot(state,red,out,&error));CHECK(out.State.Serialize()==saved);
    state=MatchState{};CHECK(!NominateColor(state,EBallType::Black,last,&error));
    state.Phase=FramePhase::ColorAfterRed;CHECK(!NominateColor(state,EBallType::Cue,last,&error));
    MatchState migrated;CHECK(MatchState::Deserialize("snooker-match-v1 1 Reds 0 0 0 Red 15 0 0 0 0",migrated,&error));
    CHECK(migrated.Serialize().find("snooker-match-v2")==0);
    CHECK(MatchState::Deserialize(last.Serialize(),migrated,&error));
    std::cout<<"PASS legal red/color scoring, nomination, last-red transition, overflow and match-v1 migration\n";
}
