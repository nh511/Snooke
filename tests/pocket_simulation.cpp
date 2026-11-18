#include <Snooker/Service/PocketSimulation.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

#define CHECK(x) do {if(!(x)){std::cerr<<"FAIL pocket simulation "<<__LINE__<<": "<<#x<<"; "<<error<<'\n';return 1;}}while(false)
using namespace Snooker;
std::string Read(const std::string& path) {std::ifstream s(path);return {std::istreambuf_iterator<char>(s),{}};}
int main(int argc,char** argv)
{
    std::string error;CHECK(argc==5);PocketSimulationConfig config;
    CHECK(TableDefinition::Deserialize(Read(argv[1]),config.Table,&error));
    CushionProfile cushion;PocketProfile profile;
    CHECK(CushionProfile::Deserialize(Read(argv[2]),cushion,&error));config.Cushions={cushion};
    CHECK(PocketProfile::Deserialize(Read(argv[3]),profile,&error));config.Pockets={profile};
    CHECK(config.IsValid(&error));TableGeometry geometry;
    CHECK(GenerateTableGeometry(config.Table,config.Cushions,config.Pockets,{},geometry,&error));
    ClothProfile cloth;CHECK(ClothProfile::Deserialize(Read(argv[4]),cloth,&error));
    CHECK(ClothProfile::Deserialize(cloth.Serialize(),cloth,&error));config.Cloth=cloth;
    const auto& p=geometry.Pockets[0];
    {
        std::ofstream s("pocket-config.txt");s<<config.Serialize();CHECK(bool(s));
    }
    PocketSimulationConfig loadedConfig;
    CHECK(PocketSimulationConfig::Deserialize(Read("pocket-config.txt"),loadedConfig,&error));
    CHECK(loadedConfig.Serialize()==config.Serialize());
    CHECK(!PocketSimulationConfig::Deserialize(config.Serialize()+" junk",loadedConfig,&error));
    CHECK(loadedConfig.Serialize()==config.Serialize());
    PhysicsClock clock;BallState ball;ball.Id=1;ball.RadiusMeters=0.01;ball.Sleeping=false;
    ball.PositionMeters=p.BottomCenter-p.Inward*profile.JawRadius+Vec3d{0,0,0.08};
    ball.LinearVelocityMetersPerSecond={0,0,-0.1};
    auto initial=PhysicsSnapshot::Capture(clock,{ball},config.Version,ESnapshotPhase::DuringStep);
    initial.PocketStates={{PocketState::InPocket,p.Definition.Id,false}};
    PocketSimulation a;CHECK(a.Restore(config,initial,&error));
    for(unsigned i=0;i<8;++i)CHECK(a.Step(&error));
    CHECK(a.SaveSnapshot().PocketStates[0].State==PocketState::InPocket);
    {
        std::ofstream s("pocket-mid.snapshot");s<<a.SaveSnapshot().Serialize();CHECK(bool(s));
    }
    PhysicsSnapshot mid;CHECK(PhysicsSnapshot::Deserialize(Read("pocket-mid.snapshot"),mid,&error));
    CHECK(mid.Serialize()==a.SaveSnapshot().Serialize());
    PocketSimulation b;CHECK(b.Restore(loadedConfig,mid,&error));
    const auto offset=a.GetEvents().size();
    for(unsigned i=0;i<120;++i) {CHECK(a.Step(&error));CHECK(b.Step(&error));CHECK(a.SaveSnapshot().Serialize()==b.SaveSnapshot().Serialize());}
    CHECK(a.SaveSnapshot().PocketStates[0].State==PocketState::Potted);
    CHECK(a.GetEvents().size()==offset+b.GetEvents().size());
    unsigned pots=0;for(std::size_t i=0;i<b.GetEvents().size();++i) {
        CHECK(a.GetEvents()[i+offset].Serialize()==b.GetEvents()[i].Serialize());
        if(b.GetEvents()[i].Kind==PocketSimulationEventKind::Potted)++pots;
    }
    CHECK(pots==1);const auto count=a.GetEvents().size();
    for(int i=0;i<100;++i)CHECK(a.Step(&error));CHECK(a.GetEvents().size()==count);
    PocketSimulation potted;CHECK(potted.Restore(config,a.SaveSnapshot(),&error));CHECK(potted.Step(&error));CHECK(potted.GetEvents().empty());
    auto invalid=mid;invalid.PocketStates[0].PocketId=999;
    const auto saved=b.SaveSnapshot().Serialize();CHECK(!b.Restore(config,invalid,&error));CHECK(b.SaveSnapshot().Serialize()==saved);
    PhysicsClock legacyClock;std::vector<BallState> legacyBalls;
    CHECK(!mid.Restore(legacyClock,legacyBalls,config.Version,&error));
    invalid=mid;invalid.PocketStates[0].PotEventEmitted=true;CHECK(!invalid.IsValid());
    invalid=mid;invalid.PocketStates[0].State=PocketState::Potted;invalid.PocketStates[0].PotEventEmitted=true;CHECK(!invalid.IsValid());
    CHECK(!PhysicsSnapshot::Deserialize(mid.Serialize()+" trailing",invalid,&error));
    // Legacy v2 migrates to v3 and has no pocket/event state.
    auto clothBall=ball;clothBall.PositionMeters={0,0,ball.RadiusMeters};clothBall.LinearVelocityMetersPerSecond={};clothBall.Sleeping=true;
    PhysicsSnapshot legacy;
    CHECK(PhysicsSnapshot::Deserialize("2 BeforeShot snooker-physics-bundle-v1 0 0 0.004166666666666667 0 1\n"+clothBall.Serialize(),legacy,&error));
    CHECK(legacy.SchemaVersion==3 && legacy.PocketStates[0].State==PocketState::OnCloth && legacy.EventCount==0);
    // Actual jaw endpoint, horizontal travel just above cloth: rebound cannot pot.
    ball.PositionMeters=p.MouthStart+p.Tangent*(-0.08)+Vec3d{0,0,ball.RadiusMeters};
    ball.LinearVelocityMetersPerSecond=p.Tangent*2;
    auto jaw=PhysicsSnapshot::Capture(clock,{ball},config.Version,ESnapshotPhase::DuringStep);
    jaw.PocketStates={{PocketState::AtMouth,p.Definition.Id,false}};
    CHECK(a.Restore(config,jaw,&error));
    for(int i=0;i<25;++i)CHECK(a.Step(&error));
    CHECK(a.SaveSnapshot().PocketStates[0].State!=PocketState::Potted);
    unsigned jawHits=0;for(const auto& e:a.GetEvents())if(e.Kind==PocketSimulationEventKind::Contact && e.Feature==PocketFeature::Jaw)++jawHits;
    CHECK(jawHits>0);CHECK(a.SaveSnapshot().Balls[0].LinearVelocityMetersPerSecond.Dot(p.Tangent)<0);
    // Physical recovery crossing the mouth back onto cloth.
    ball.PositionMeters=p.Definition.Center-p.Inward*0.001+Vec3d{0,0,ball.RadiusMeters};
    ball.LinearVelocityMetersPerSecond=p.Inward*2;initial.Balls={ball};initial.PocketStates={{PocketState::InPocket,p.Definition.Id,false}};
    CHECK(a.Restore(config,initial,&error));CHECK(a.Step(&error));
    CHECK(a.SaveSnapshot().PocketStates[0].State==PocketState::Ejected);
    unsigned recovery=0;for(const auto& e:a.GetEvents())if(e.Kind==PocketSimulationEventKind::RecoveredToCloth)++recovery;
    CHECK(recovery==1);
    // Invalid states fail without modifying the destination snapshot.
    invalid=initial;invalid.Balls[0].PositionMeters.X=std::numeric_limits<double>::quiet_NaN();
    const auto unchanged=a.SaveSnapshot().Serialize();CHECK(!a.Restore(config,invalid,&error));CHECK(a.SaveSnapshot().Serialize()==unchanged);
    // Real cloth-to-mouth crossing, not caller-supplied pot observation.
    ball.PositionMeters=p.Definition.Center+p.Inward*0.001+Vec3d{0,0,ball.RadiusMeters};
    ball.LinearVelocityMetersPerSecond=p.Inward*(-0.5);
    initial=PhysicsSnapshot::Capture(clock,{ball},config.Version,ESnapshotPhase::DuringStep);
    CHECK(a.Restore(config,initial,&error));CHECK(a.Step(&error));
    CHECK(a.SaveSnapshot().PocketStates[0].State==PocketState::InPocket);
    CHECK(a.GetEvents()[0].Kind==PocketSimulationEventKind::MouthEntered);
    for(int i=0;i<1000 && a.SaveSnapshot().PocketStates[0].State!=PocketState::Potted;++i)CHECK(a.Step(&error));
    CHECK(a.SaveSnapshot().PocketStates[0].State==PocketState::Potted);
    std::cout<<"PASS pocket configuration/file replay, v2 migration, exact mid-pocket continuation, once-only pot, jaw and recovery\n";
}
