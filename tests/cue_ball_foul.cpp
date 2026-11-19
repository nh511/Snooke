#include <Snooker/Rules/CueBallFoul.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL cue foul "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
std::string Read(const char* p){std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
ShotPhysicsResult Fixture(bool cuePotted,bool objectPotted)
{
    ShotPhysicsResult r;r.CueBall=1;r.FirstContact=2;
    BallState cue;cue.Id=1;cue.PositionMeters={0,0,cue.RadiusMeters};auto object=cue;object.Id=2;object.PositionMeters.X=.2;
    PhysicsClock clock;r.Initial=PhysicsSnapshot::Capture(clock,{cue,object},"foul-v1",ESnapshotPhase::BeforeShot);
    r.Final=r.Initial;r.Final.StepIndex=100;r.Final.SimulationTimeSeconds=100.0/240;r.Final.Phase=ESnapshotPhase::AfterShot;
    BallDefinition c;c.Id=1;c.Type=EBallType::Cue;c.InitialPositionMeters=cue.PositionMeters;auto o=c;o.Id=2;o.Type=EBallType::Red;o.InitialPositionMeters=object.PositionMeters;r.Definitions={c,o};
    r.Events={{0,0,0,PhysicsEventKind::CueStrike,1,0,{},TableNormal,.1},{1,10,.04,PhysicsEventKind::BallBall,1,2,{},TableNormal,.1}};
    if(objectPotted){r.Potted.push_back(2);r.Final.PocketStates[1]={PocketState::Potted,1,true};r.Events.push_back({2,50,.2,PhysicsEventKind::Potted,2,1,{},{},0});}
    if(cuePotted){r.CueBallPotted=true;r.Potted.push_back(1);r.Final.PocketStates[0]={PocketState::Potted,1,true};r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),60,.25,PhysicsEventKind::Potted,1,1,{},{},0});}
    r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),100,r.Final.SimulationTimeSeconds,PhysicsEventKind::ShotSettled,0,0,{},{},0});r.Final.EventCount=r.Events.size();return r;
}
int main(int argc,char** argv)
{
    std::string error;CHECK(argc==2);TableDefinition table;CHECK(TableDefinition::Deserialize(Read(argv[1]),table,&error));
    CueBallFoulResult foul;auto none=Fixture(false,false);CHECK(none.IsValid(&error));CHECK(EvaluateCueBallPotted(none,foul,&error));CHECK(!foul.Foul&&foul.ObjectBallsPotted.empty());
    auto cue=Fixture(true,true);CHECK(EvaluateCueBallPotted(cue,foul,&error));CHECK(foul.Foul&&foul.CueBallInHand&&foul.SuppressObjectScoring&&foul.ObjectBallsPotted==std::vector<BallId>{2});CHECK(foul.IsValid(&error));
    auto savedCueResult=foul;auto validNone=Fixture(false,false);CHECK(EvaluateCueBallPotted(validNone,foul,&error));CHECK(!foul.Foul);CHECK(savedCueResult.CueBall==1 && savedCueResult.CueBallInHand);
    BallDefinition ball;ball.Id=1;ball.Type=EBallType::Cue;ball.InitialPositionMeters={0,0,.02625};
    CueBallPlacementRules rules;BallDefinition placed;
    CHECK(PlaceCueBallInD(table,ball,{-1.2,0,.02625},{},rules,placed,&error));CHECK(placed.InitialPositionMeters.NearlyEquals({-1.2,0,.02625}));
    BallDefinition red;red.Id=2;red.Type=EBallType::Red;red.InitialPositionMeters={-1.2,.01,.02625};CHECK(!PlaceCueBallInD(table,ball,{-1.2,0,.02625},{red},rules,placed,&error));
    CHECK(!PlaceCueBallInD(table,ball,{0,0,.02625},{},rules,placed,&error));CHECK(!PlaceCueBallInD(table,ball,{-1.2,.3,.02625},{},rules,placed,&error));
    CHECK(!PlaceCueBallInD(table,red,{-1.2,0,.02625},{},rules,placed,&error));
    std::cout<<"PASS cue-ball foul/no-score result, ball-in-hand D placement, overlap and invalid position rejection\n";
}
