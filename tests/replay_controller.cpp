#include <Snooker/Replay/ReplayController.h>
#include <fstream>
#include <iterator>
#include <iostream>
#include <limits>

#define CHECK(x) do{if(!(x)){std::cerr<<"FAIL replay controller "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
int main(int argc,char** argv)
{
    using namespace Snooker;std::string error;CHECK(argc==2);std::ifstream f(argv[1]);CHECK(bool(f));
    const std::string text{std::istreambuf_iterator<char>(f),{}};PhysicsShotRecord record;
    CHECK(PhysicsShotRecord::Deserialize(text,record,&error));ReplayController controller;
    CHECK(controller.Load(record,&error));CHECK(controller.CheckpointCount()<record.Final.StepIndex/100+3);
    auto live=CreatePhysicsService(record.Parameters,&error);CHECK(live);CHECK(live->BeginShot(record.Initial,record.Input,&error));
    PhysicsRunState state;
    for(unsigned i=0;i<record.Final.StepIndex-record.Initial.StepIndex;++i) {
        CHECK(live->Step(record.Parameters.FixedStep,state,&error));
        const auto expected=live->SaveSnapshot();CHECK(controller.SeekStep(expected.StepIndex,&error));
        CHECK(controller.Snapshot().Serialize()==expected.Serialize());
    }
    CHECK(controller.SeekTime(record.Final.SimulationTimeSeconds,&error));CHECK(controller.Snapshot().Serialize()==record.Final.Serialize());
    CHECK(controller.SeekEvent(0,&error));CHECK(controller.Snapshot().StepIndex==record.Initial.StepIndex);
    for(std::size_t i=record.Events.size();i>0;--i){CHECK(controller.SeekEvent(i-1,&error));CHECK(controller.Snapshot().StepIndex==record.Events[i-1].Step);}
    const auto saved=controller.Snapshot().Serialize();CHECK(!controller.SeekEvent(record.Events.size(),&error));CHECK(controller.Snapshot().Serialize()==saved);
    CHECK(!controller.SeekStep(record.Final.StepIndex+1,&error));CHECK(!controller.SeekTime(std::numeric_limits<double>::quiet_NaN(),&error));
    CHECK(!controller.SeekFault(&error));auto corrupt=record;corrupt.Events[0].Impulse+=1e-6;
    CHECK(!controller.Load(corrupt,&error));CHECK(controller.Snapshot().Serialize()==saved);
    CHECK(controller.SeekTime(record.Parameters.FixedStep*1.1,&error));CHECK(controller.Snapshot().StepIndex==2);
    // Real collision and pot, not invented replay events.
    auto initial=record.Initial;auto cue=initial.Balls[0],target=cue;
    cue.PositionMeters={0,-0.6,cue.RadiusMeters};target.Id=2;target.PositionMeters={0,-0.78,target.RadiusMeters};
    initial.Balls={cue,target};initial.PocketStates.resize(2);
    ShotInput input;input.CueBallId=1;input.CueDirection={0,-1,0};input.ForwardSpeedMetersPerSecond=1;
    live=CreatePhysicsService(record.Parameters,&error);CHECK(live->BeginShot(initial,input,&error));
    state=PhysicsRunState::Running;
    while(state==PhysicsRunState::Running){CHECK(live->Step(record.Parameters.FixedStep,state,&error));CHECK(state!=PhysicsRunState::Faulted);}
    PhysicsShotRecord pot;pot.Parameters=record.Parameters;pot.Initial=initial;pot.Input=input;pot.Final=live->SaveSnapshot();pot.Events=live->GetEvents();
    CHECK(pot.IsValid(&error));CHECK(controller.Load(pot,&error));bool contact=false,potted=false;
    for(std::size_t i=0;i<pot.Events.size();++i) {
        if(pot.Events[i].Kind==PhysicsEventKind::BallBall && !contact){CHECK(controller.SeekEvent(i,&error));CHECK(controller.Snapshot().StepIndex==pot.Events[i].Step);contact=true;}
        if(pot.Events[i].Kind==PhysicsEventKind::Potted && !potted){CHECK(controller.SeekEvent(i,&error));CHECK(controller.Snapshot().PocketStates[1].State==PocketState::Potted);potted=true;}
    }
    CHECK(contact && potted);
    // Fault seek selects the actual last committed state before the failed step.
    auto config=record.Parameters;config.Safety.MaxSimulationTimeSeconds=config.FixedStep/2;
    live=CreatePhysicsService(config,&error);CHECK(live->BeginShot(record.Initial,record.Input,&error));
    CHECK(live->Step(config.FixedStep,state,&error));CHECK(state==PhysicsRunState::Faulted);
    auto fault=record;fault.Parameters=config;fault.Events=live->GetEvents();fault.Final=live->SaveSnapshot();fault.Fault=live->GetFault();fault.Result="fault:Timeout";
    CHECK(fault.IsValid(&error));CHECK(controller.Load(fault,&error));CHECK(controller.SeekFault(&error));
    CHECK(controller.Snapshot().Serialize()==fault.Fault.Snapshot.Serialize());
    std::cout<<"PASS checkpoint-based replay step/time/event seeks and transactional errors\n";
}
