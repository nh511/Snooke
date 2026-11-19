#include <Snooker/Rules/Miss.h>
#include <fstream>
#include <iterator>
#include <iostream>
#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL miss "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
std::string Read(const char*p){std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
using namespace Snooker;
ShotPhysicsResult Shot(bool blocked)
{
 ShotPhysicsResult r;r.CueBall=1;r.FirstContact=0;std::vector<BallState> balls;std::vector<BallDefinition> defs;auto add=[&](BallId id,EBallType t,double x,double y){BallState b;b.Id=id;b.PositionMeters={x,y,b.RadiusMeters};balls.push_back(b);BallDefinition d;d.Id=id;d.Type=t;d.InitialPositionMeters=b.PositionMeters;defs.push_back(d);};add(1,EBallType::Cue,0,0);add(2,EBallType::Red,1,0);if(blocked){add(3,EBallType::Black,.5,.06);add(4,EBallType::Pink,.5,-.06);}PhysicsClock c;r.Initial=PhysicsSnapshot::Capture(c,balls,"miss-v1",ESnapshotPhase::BeforeShot);r.Final=r.Initial;r.Final.StepIndex=10;r.Final.SimulationTimeSeconds=10.0/240;r.Final.Phase=ESnapshotPhase::AfterShot;r.Definitions=defs;r.Events={{0,0,0,PhysicsEventKind::CueStrike,1,0,{},{},.1},{1,10,r.Final.SimulationTimeSeconds,PhysicsEventKind::ShotSettled,0,0,{},{},0}};r.Final.EventCount=2;return r;
}
int main(int argc,char**argv){std::string error;CHECK(argc==3);RulesProfile match,practice;CHECK(RulesProfile::Deserialize(Read(argv[1]),match,&error));CHECK(RulesProfile::Deserialize(Read(argv[2]),practice,&error));CHECK(match.MissEnabled&&!practice.MissEnabled);MatchState state;FirstContactResult contact{FirstContactKind::NoContact,1,0,EBallType::Red,EBallType::Red};MissResult result;auto clear=Shot(false);CHECK(EvaluateMiss(state,clear,contact,match,result,&error));CHECK(result.Foul&&result.Miss&&result.ClearFullBallAvailable&&!result.NeedsSnookers&&result.State.MissPending);
 MissResolution resolution;CHECK(ResolveMiss(result,MissChoice::RestoreAndReplay,clear.Initial,clear.Final,resolution,&error));CHECK(resolution.Restored&&resolution.PlayerToPlay==0&&resolution.Physics.Serialize()==clear.Initial.Serialize());CHECK(ResolveMiss(result,MissChoice::PlayFromCurrent,clear.Initial,clear.Final,resolution,&error));CHECK(!resolution.Restored&&resolution.PlayerToPlay==1&&resolution.Physics.Serialize()==clear.Final.Serialize());
 CHECK(EvaluateMiss(state,clear,contact,practice,result,&error));CHECK(result.Foul&&!result.Miss&&!result.State.MissPending);auto blocked=Shot(true);CHECK(EvaluateMiss(state,blocked,contact,match,result,&error));CHECK(!result.Miss&&!result.ClearFullBallAvailable);
 state.Scores={0,200};state.RemainingReds=1;CHECK(EvaluateMiss(state,clear,contact,match,result,&error));CHECK(result.NeedsSnookers&&!result.Miss);
 state=MatchState{};contact={FirstContactKind::LegalBallOn,1,2,EBallType::Red,EBallType::Red};CHECK(EvaluateMiss(state,clear,contact,match,result,&error));CHECK(!result.Foul&&!result.Miss);
 const auto saved=resolution.Physics.Serialize();CHECK(!ResolveMiss(result,MissChoice::RestoreAndReplay,clear.Initial,clear.Final,resolution,&error));CHECK(resolution.Physics.Serialize()==saved);
 std::cout<<"PASS match/practice Miss, clear full-ball path, snookers-needed suppression and restore/current choices\n";}
