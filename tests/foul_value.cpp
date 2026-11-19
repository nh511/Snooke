#include <Snooker/Rules/FoulValue.h>
#include <iostream>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL foul value "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;

ShotPhysicsResult Make(EBallType first,bool cue=false,bool illegalPot=false)
{
    ShotPhysicsResult r;r.CueBall=1;r.FirstContact=first==EBallType::Cue?0:2;r.CueBallPotted=cue;
    BallState a;a.Id=1;a.PositionMeters={0,0,a.RadiusMeters};auto b=a;b.Id=2;b.PositionMeters.X=.2;
    PhysicsClock c;r.Initial=PhysicsSnapshot::Capture(c,{a,b},"foul-v1",ESnapshotPhase::BeforeShot);
    r.Final=r.Initial;r.Final.StepIndex=100;r.Final.SimulationTimeSeconds=100.0/240;r.Final.Phase=ESnapshotPhase::AfterShot;
    BallDefinition x;x.Id=1;x.Type=EBallType::Cue;x.InitialPositionMeters=a.PositionMeters;auto y=x;y.Id=2;y.Type=first;r.Definitions={x,y};
    r.Events={{0,0,0,PhysicsEventKind::CueStrike,1,0,{},{},.1}};
    if(r.FirstContact)r.Events.push_back({1,1,.01,PhysicsEventKind::BallBall,1,2,{},{},.1});
    if(cue){r.Potted.push_back(1);r.Final.PocketStates[0]={PocketState::Potted,1,true};r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),2,.02,PhysicsEventKind::Potted,1,1,{},{},0});}
    if(illegalPot){r.Potted.push_back(2);r.Final.PocketStates[1]={PocketState::Potted,1,true};r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),3,.03,PhysicsEventKind::Potted,2,1,{},{},0});}
    r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),100,r.Final.SimulationTimeSeconds,PhysicsEventKind::ShotSettled,0,0,{},{},0});
    r.Final.EventCount=r.Events.size();return r;
}
int main()
{
    std::string error;MatchState state;FirstContactResult first;CueBallFoulResult cue;FoulValueResult out;
    auto legal=Make(EBallType::Red);first={FirstContactKind::LegalBallOn,1,2,EBallType::Red,EBallType::Red};cue={false,false,false,1,{},{}};
    CHECK(CalculateFoulValue(state,legal,first,cue,out,&error));CHECK(!out.Foul);
    auto wrong=Make(EBallType::Black);first={FirstContactKind::WrongBall,1,2,EBallType::Red,EBallType::Black};
    CHECK(CalculateFoulValue(state,wrong,first,cue,out,&error));CHECK(out.Foul&&out.Points==7&&out.AwardedToPlayer==1);
    auto none=Make(EBallType::Red);none.FirstContact=0;none.Events.erase(none.Events.begin()+1);none.Events.back().Sequence=1;none.Final.EventCount=none.Events.size();
    first={FirstContactKind::NoContact,1,0,EBallType::Red,EBallType::Red};CHECK(CalculateFoulValue(state,none,first,cue,out,&error));CHECK(out.Foul&&out.Points==4);
    auto cueInput=Make(EBallType::Red,true,true);first={FirstContactKind::LegalBallOn,1,2,EBallType::Red,EBallType::Red};cue={true,true,true,1,{2},"cue ball potted"};
    CHECK(CalculateFoulValue(state,cueInput,first,cue,out,&error));CHECK(out.Points==4);
    state.CurrentPlayer=1;CHECK(CalculateFoulValue(state,wrong,first,cue,out,&error));CHECK(out.AwardedToPlayer==0);
    state.FreeBallAvailable=true;CHECK(!CalculateFoulValue(state,wrong,first,cue,out,&error));
    std::cout<<"PASS foul minimum/relevant value, no contact, cue pot, opponent award and free-ball rejection\n";
}
