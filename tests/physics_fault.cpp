#include <Snooker/Physics/PhysicsFault.h>
#include <Snooker/Service/PocketSimulation.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL physics fault "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
std::string Read(const char* path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char** argv)
{
    std::string error;CHECK(argc==4);TableDefinition table;CushionProfile cushion;PocketProfile pocket;
    CHECK(TableDefinition::Deserialize(Read(argv[1]),table,&error));CHECK(CushionProfile::Deserialize(Read(argv[2]),cushion,&error));CHECK(PocketProfile::Deserialize(Read(argv[3]),pocket,&error));
    TableGeometry geometry;CHECK(GenerateTableGeometry(table,{cushion},{pocket},{},geometry,&error));
    BallState ball;ball.Id=1;ball.RadiusMeters=0.01;ball.PositionMeters={0,0,0.01};ball.Sleeping=false;
    PhysicsClock clock;auto snapshot=PhysicsSnapshot::Capture(clock,{ball},"fault-v1",ESnapshotPhase::DuringStep);
    snapshot.PocketStates={{PocketState::OnCloth,0,false}};
    PhysicsSafetyLimits limits;PhysicsFault fault;
    CHECK(CheckPhysicsSafety(snapshot,geometry,limits,0,fault,&error));
    auto expect=[&](PhysicsFaultCode code,PhysicsSnapshot candidate,std::uint64_t events=0)->bool {
        PhysicsFault result,loaded;
        return !CheckPhysicsSafety(candidate,geometry,limits,events,result,&error) && result.Code==code &&
            result.IsValid(&error) && PhysicsFault::Deserialize(result.Serialize(),loaded,&error) && loaded.Serialize()==result.Serialize();
    };
    auto bad=snapshot;bad.Balls[0].PositionMeters.X=std::numeric_limits<double>::quiet_NaN();CHECK(expect(PhysicsFaultCode::NonFinite,bad));
    CHECK(!CheckPhysicsSafety(bad,geometry,limits,0,fault,&error));CHECK(fault.CandidateDiagnostic.find("nan")!=std::string::npos);
    bad=snapshot;bad.AccumulatorSeconds=std::numeric_limits<double>::infinity();CHECK(expect(PhysicsFaultCode::NonFinite,bad));
    bad=snapshot;bad.Balls[0].PositionMeters.Z=0;CHECK(expect(PhysicsFaultCode::Penetration,bad));
    bad=snapshot;bad.Balls[0].LinearVelocityMetersPerSecond={51,0,0};CHECK(expect(PhysicsFaultCode::VelocityExplosion,bad));
    bad=snapshot;bad.StepIndex=29040;bad.SimulationTimeSeconds=bad.StepIndex*bad.FixedStepSeconds;CHECK(expect(PhysicsFaultCode::Timeout,bad));
    CHECK(expect(PhysicsFaultCode::EventLoop,snapshot,257));
    bad=snapshot;bad.Balls[0].PositionMeters.X=2;CHECK(expect(PhysicsFaultCode::LeftTable,bad));
    bad=snapshot;bad.PocketStates={{PocketState::InPocket,1,false}};
    bad.Balls[0].PositionMeters={0,0,geometry.Pockets[0].BottomCenter.Z-limits.PocketEscapeDepthMeters-0.01};
    CHECK(expect(PhysicsFaultCode::PocketEscape,bad));
    bad=snapshot;bad.PocketStates={{PocketState::InPocket,999,false}};CHECK(expect(PhysicsFaultCode::SolverError,bad));
    bad=snapshot;auto other=bad.Balls[0];other.Id=2;bad.Balls.push_back(other);bad.PocketStates.push_back({});CHECK(expect(PhysicsFaultCode::Penetration,bad));
    PhysicsSafetyLimits round;CHECK(PhysicsSafetyLimits::Deserialize(limits.Serialize(),round,&error));CHECK(round.Serialize()==limits.Serialize());
    for(const auto name:{"NonFinite","Penetration","VelocityExplosion","Timeout","EventLoop","LeftTable","PocketEscape","SolverError"}) {PhysicsFaultCode code;CHECK(TryParsePhysicsFaultCode(name,code));CHECK(std::string(PhysicsFaultCodeName(code))==name);}
    CHECK(!PhysicsSafetyLimits::Deserialize(limits.Serialize()+" tail",round,&error));
    // An actual service fault freezes the service and writes a replayable fault file.
    PocketSimulationConfig config;config.Table=table;config.Cushions={cushion};config.Pockets={pocket};
    config.Version="fault-v1";config.Safety.MaxLinearSpeedMetersPerSecond=0.001;config.Cloth={"cloth-v1","test",0.2,0.01,0.5,0.001,0.01,0.00001,0.25};
    CHECK(config.IsValid(&error));snapshot.PhysicsParameterVersion=config.Version;snapshot.Balls[0].LinearVelocityMetersPerSecond={0.1,0,0};
    PocketSimulation service;CHECK(service.Restore(config,snapshot,&error));const auto before=service.SaveSnapshot().Serialize();CHECK(!service.Step(&error));CHECK(service.IsFaulted());CHECK(service.GetFault().Code==PhysicsFaultCode::VelocityExplosion);CHECK(service.SaveSnapshot().Serialize()==before);
    CHECK(service.SaveFault("physics-fault.txt",&error));PhysicsFault saved;CHECK(PhysicsFault::Deserialize(Read("physics-fault.txt"),saved,&error));CHECK(saved.Code==PhysicsFaultCode::VelocityExplosion);
    const auto faultState=service.SaveSnapshot().Serialize();CHECK(!service.Step(&error));CHECK(service.SaveSnapshot().Serialize()==faultState);
    PocketSimulationConfig replayConfig;CHECK(PocketSimulationConfig::Deserialize(saved.ReplayConfig,replayConfig,&error));
    PocketSimulation replay;CHECK(replay.Restore(replayConfig,saved.Snapshot,&error));CHECK(!replay.Step(&error));
    CHECK(replay.GetFault().Serialize()==saved.Serialize());
    config.Safety.MaxLinearSpeedMetersPerSecond=50;config.Safety.MaxSimulationTimeSeconds=config.FixedStep/2;
    CHECK(service.Restore(config,snapshot,&error));CHECK(!service.Step(&error));CHECK(service.GetFault().Code==PhysicsFaultCode::Timeout);
    CHECK(service.SaveSnapshot().Serialize()==snapshot.Serialize());
    CHECK(service.SaveFault("physics-timeout.txt",&error));
    CHECK(PocketSimulationConfig::Deserialize(service.GetFault().ReplayConfig,replayConfig,&error));
    CHECK(replay.Restore(replayConfig,service.GetFault().Snapshot,&error));CHECK(!replay.Step(&error));
    CHECK(replay.GetFault().Serialize()==service.GetFault().Serialize());
    // Two real events (bottom contact + pot) exceed a one-event step budget.
    config.Safety.MaxSimulationTimeSeconds=120;config.Safety.MaxEventsPerStep=1;
    const auto& p=geometry.Pockets[0];auto descent=snapshot;
    descent.PocketStates={{PocketState::InPocket,p.Definition.Id,false}};
    descent.Balls[0].PositionMeters=p.BottomCenter-p.Inward*p.Parameters.JawRadius+Vec3d{0,0,0.011};
    descent.Balls[0].LinearVelocityMetersPerSecond={0,0,-1};
    CHECK(service.Restore(config,descent,&error));CHECK(!service.Step(&error));CHECK(service.GetFault().Code==PhysicsFaultCode::EventLoop);
    CHECK(service.GetEvents().empty());CHECK(service.SaveSnapshot().Serialize()==descent.Serialize());
    CHECK(service.SaveFault("physics-eventloop.txt",&error));
    CHECK(PocketSimulationConfig::Deserialize(service.GetFault().ReplayConfig,replayConfig,&error));
    CHECK(replay.Restore(replayConfig,service.GetFault().Snapshot,&error));CHECK(!replay.Step(&error));
    CHECK(replay.GetFault().Serialize()==service.GetFault().Serialize());
    config.Safety.MaxEventsPerStep=2;
    CHECK(service.Restore(config,descent,&error));CHECK(service.Step(&error));CHECK(service.GetEvents().size()==2);
    // Cumulative history is not the per-step budget: later steps add no events.
    for(int i=0;i<20;++i)CHECK(service.Step(&error));CHECK(!service.IsFaulted());
    const auto good=service.GetEvents().size();CHECK(good==2);
    const auto original=saved.Serialize();CHECK(!PhysicsFault::Deserialize(original+" tail",saved,&error));
    CHECK(saved.Serialize()==original);
    std::cout<<"PASS physics fault codes, limits, transactional snapshots, fault serialization and frozen service\n";
}
