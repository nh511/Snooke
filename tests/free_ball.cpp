#include <Snooker/Rules/FreeBall.h>
#include <iostream>
#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL free ball "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
struct Scene{PhysicsSnapshot Snapshot;std::vector<BallDefinition> Definitions;};
Scene MakeScene(bool left,bool right)
{
    std::vector<BallState> balls;std::vector<BallDefinition> defs;
    auto add=[&](BallId id,EBallType type,double x,double y){BallState b;b.Id=id;b.PositionMeters={x,y,b.RadiusMeters};balls.push_back(b);BallDefinition d;d.Id=id;d.Type=type;d.InitialPositionMeters=b.PositionMeters;defs.push_back(d);};
    add(1,EBallType::Cue,0,0);add(2,EBallType::Red,1,0);if(left)add(3,EBallType::Black,.5,.06);if(right)add(4,EBallType::Pink,.5,-.06);
    PhysicsClock c;return {PhysicsSnapshot::Capture(c,balls,"free-v1",ESnapshotPhase::BeforeShot),defs};
}
ShotPhysicsResult FreeShot(bool pot,bool createSnooker)
{
    auto scene=MakeScene(false,false);auto black=scene.Snapshot.Balls[0];black.Id=3;black.PositionMeters={.5,createSnooker?0:.2,black.RadiusMeters};
    scene.Snapshot.Balls.push_back(black);scene.Snapshot.PocketStates.push_back({});BallDefinition d;d.Id=3;d.Type=EBallType::Black;d.InitialPositionMeters=black.PositionMeters;scene.Definitions.push_back(d);
    ShotPhysicsResult r;r.CueBall=1;r.FirstContact=3;r.Definitions=scene.Definitions;r.Initial=scene.Snapshot;r.Final=r.Initial;r.Final.StepIndex=20;r.Final.SimulationTimeSeconds=20.0/240;r.Final.Phase=ESnapshotPhase::AfterShot;
    r.Events={{0,0,0,PhysicsEventKind::CueStrike,1,0,{},{},.1},{1,5,.01,PhysicsEventKind::BallBall,1,3,{},{},.1}};
    if(pot){r.Potted={3};r.Final.PocketStates[2]={PocketState::Potted,1,true};r.Events.push_back({2,10,.02,PhysicsEventKind::Potted,3,1,{},{},0});}
    r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),20,r.Final.SimulationTimeSeconds,PhysicsEventKind::ShotSettled,0,0,{},{},0});r.Final.EventCount=r.Events.size();return r;
}
int main()
{
    std::string error;SnookeredResult query;auto clear=MakeScene(false,false);CHECK(QuerySnookered(clear.Snapshot,clear.Definitions,1,EBallType::Red,query,0,&error));CHECK(!query.Snookered);
    auto one=MakeScene(true,false);CHECK(QuerySnookered(one.Snapshot,one.Definitions,1,EBallType::Red,query,0,&error));CHECK(!query.Snookered);
    auto blocked=MakeScene(true,true);CHECK(QuerySnookered(blocked.Snapshot,blocked.Definitions,1,EBallType::Red,query,0,&error));CHECK(query.Snookered);
    MatchState state,next;CHECK(OfferFreeBall(state,blocked.Snapshot,blocked.Definitions,1,next,&error));CHECK(next.FreeBallAvailable);MatchState nominated;CHECK(NominateFreeBall(next,EBallType::Black,nominated,&error));CHECK(nominated.FreeBallNominated&&nominated.FreeBallType==EBallType::Black);CHECK(!NominateFreeBall(nominated,EBallType::Red,state,&error));
    FreeBallShotResult result;auto potted=FreeShot(true,false);CHECK(potted.IsValid(&error));CHECK(EvaluateFreeBallShot(nominated,potted,result,&error));const std::vector<BallId> black{3};CHECK(!result.Foul&&result.Points==1&&result.ColorsToRespot==black&&!result.State.FreeBallAvailable);
    auto snooker=FreeShot(false,true);CHECK(EvaluateFreeBallShot(nominated,snooker,result,&error));CHECK(result.Foul&&result.Reason=="Free Ball created a snooker");
    auto wrong=potted;wrong.FirstContact=2;CHECK(!wrong.IsValid(&error));
    state=nominated;state.Phase=FramePhase::FinalColors;state.RemainingReds=0;state.BallOn=EBallType::Pink;state.FreeBallType=EBallType::Black;snooker.Definitions[1].Type=EBallType::Pink;CHECK(EvaluateFreeBallShot(state,snooker,result,&error));CHECK(!result.Foul);
    std::cout<<"PASS snookered paths, offer/nomination, Ball On value/respot and free-ball snooker restriction\n";
}
