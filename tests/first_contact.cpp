#include <Snooker/Rules/FirstContact.h>
#include <iostream>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL first contact "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
ShotPhysicsResult Fixture(EBallType type,BallId first)
{
    ShotPhysicsResult r;r.CueBall=1;r.FirstContact=first;BallState cue;cue.Id=1;cue.PositionMeters={0,0,cue.RadiusMeters};auto ball=cue;ball.Id=2;ball.PositionMeters.X=.2;
    PhysicsClock clock;r.Initial=PhysicsSnapshot::Capture(clock,{cue,ball},"contact-v1",ESnapshotPhase::BeforeShot);r.Final=r.Initial;r.Final.StepIndex=1;r.Final.SimulationTimeSeconds=r.Final.FixedStepSeconds;r.Final.Phase=ESnapshotPhase::AfterShot;
    BallDefinition c;c.Id=1;c.Type=EBallType::Cue;c.InitialPositionMeters=cue.PositionMeters;auto b=c;b.Id=2;b.Type=type;b.InitialPositionMeters=ball.PositionMeters;r.Definitions={c,b};
    r.Events={{0,0,0,PhysicsEventKind::CueStrike,1,0,{},{},.1}};if(first)r.Events.push_back({1,1,r.Final.SimulationTimeSeconds,PhysicsEventKind::BallBall,1,first,{},{},.1});r.Events.push_back({static_cast<std::uint64_t>(r.Events.size()),1,r.Final.SimulationTimeSeconds,PhysicsEventKind::ShotSettled,0,0,{},{},0});r.Final.EventCount=r.Events.size();return r;
}
int main()
{
    std::string error;MatchState state;FirstContactResult result;
    auto legal=Fixture(EBallType::Red,2);CHECK(EvaluateFirstContact(state,legal,result,&error));CHECK(result.Kind==FirstContactKind::LegalBallOn&&!result.IsFoul());CHECK(result.ContactedBall==2);
    auto wrong=Fixture(EBallType::Black,2);CHECK(EvaluateFirstContact(state,wrong,result,&error));CHECK(result.Kind==FirstContactKind::WrongBall&&result.IsFoul());CHECK(result.Actual==EBallType::Black);
    auto none=Fixture(EBallType::Red,0);CHECK(EvaluateFirstContact(state,none,result,&error));CHECK(result.Kind==FirstContactKind::NoContact&&result.IsFoul());
    state.Phase=FramePhase::ColorAfterRed;state.BallOn=EBallType::Blue;auto color=Fixture(EBallType::Blue,2);CHECK(EvaluateFirstContact(state,color,result,&error));CHECK(!result.IsFoul());
    const auto savedKind=result.Kind;const auto savedBall=result.ContactedBall;auto bad=wrong;bad.FirstContact=99;CHECK(!EvaluateFirstContact(state,bad,result,&error));CHECK(result.Kind==savedKind&&result.ContactedBall==savedBall);
    state.ShotInProgress=true;CHECK(!EvaluateFirstContact(state,legal,result,&error));state=MatchState{};state.Phase=FramePhase::Finished;CHECK(!EvaluateFirstContact(state,legal,result,&error));
    std::cout<<"PASS legal Ball On, wrong target, no-contact, color phase and transactional rejection\n";
}
