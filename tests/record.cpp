#include <Snooker/Replay/ShotRecord.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL record line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main(int argc,char** argv)
{
    using namespace Snooker;
    CHECK(argc==2);
    std::ifstream example(argv[1]); CHECK(bool(example));
    std::string text((std::istreambuf_iterator<char>(example)),{}), error;
    ShotRecord r;
    CHECK(ShotRecord::Deserialize(text,r,&error)); CHECK(r.Result=="schema-fixture");
    PhysicsClock clock; std::vector<BallState> balls=r.Initial.Balls;
    balls[0].Sleeping=false; balls[0].LinearVelocityMetersPerSecond={1,0,0};
    r.Initial=PhysicsSnapshot::Capture(clock,balls,r.Parameters.ParameterVersion,ESnapshotPhase::BeforeShot);
    CHECK(clock.Advance(0.2,[&](Seconds dt){balls[0].PositionMeters+=balls[0].LinearVelocityMetersPerSecond*dt;}));
    r.Final=PhysicsSnapshot::Capture(clock,balls,r.Parameters.ParameterVersion,ESnapshotPhase::AfterShot);
    r.BuildVersion="test-build-v1"; r.Result="uniform-motion-test";
    r.Events={{0,0,"start",1,0,{0,0,0},{0,0,1}},
              {clock.State().CompletedSteps,clock.SimulationTime(),"sample",1,0,balls[0].PositionMeters,{0,0,1}}};
    CHECK(r.IsValid(&error));
    {
        std::ofstream file("shot-record.txt"); CHECK(bool(file)); CHECK(bool(file<<r.Serialize()));
    }
    std::ifstream file("shot-record.txt"); CHECK(bool(file));
    text.assign(std::istreambuf_iterator<char>(file),{});
    ShotRecord loaded;
    CHECK(ShotRecord::Deserialize(text,loaded,&error)); CHECK(r.Serialize()==loaded.Serialize());
    const auto original=loaded.Serialize();
    for(const auto& bad:{text+"junk",text.substr(0,text.find("final")),std::string("snooker-shot-v2")}) {
        CHECK(!ShotRecord::Deserialize(bad,loaded,&error)); CHECK(!error.empty()); CHECK(loaded.Serialize()==original);
    }
    auto invalid=r; invalid.Parameters.ParameterVersion="other-v1"; CHECK(!invalid.IsValid());
    invalid=r; invalid.Final.FixedStepSeconds*=2; CHECK(!invalid.IsValid());
    invalid=r; invalid.Events[0].Ball=99; CHECK(!invalid.IsValid());
    invalid=r; std::swap(invalid.Events[0],invalid.Events[1]); CHECK(!invalid.IsValid());
    invalid=r; invalid.Input.PullbackDistanceMeters=100; CHECK(!invalid.IsValid());
    CHECK(!ShotRecord::Deserialize(invalid.Serialize(),loaded,&error)); CHECK(loaded.Serialize()==original);
    PhysicsClock restored; std::vector<BallState> restoredBalls;
    CHECK(loaded.Initial.Restore(restored,restoredBalls,loaded.Parameters.ParameterVersion));
    CHECK(restoredBalls[0].Serialize()==r.Initial.Balls[0].Serialize());
    std::cout << "PASS ShotRecord file/config/input/events/results round trip and rejection tests\n";
}
