#include <Snooker/Physics/PhysicsSnapshot.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL snapshot line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    PhysicsClock clock;
    BallState ball; ball.Id=1; ball.Sleeping=false; ball.LinearVelocityMetersPerSecond={0.3,-0.1,0};
    ball.AngularVelocityRadiansPerSecond={2,3,-1}; ball.SleepTimeSeconds=0.05;
    ball.Contact={EContactKind::Cloth,0,TableNormal};
    std::vector<BallState> balls{ball};
    const auto step=[&](Seconds dt){ balls[0].PositionMeters+=balls[0].LinearVelocityMetersPerSecond*dt; };
    CHECK(clock.Advance(0.137,step));
    CHECK(clock.State().Accumulator>0);
    auto snapshot=PhysicsSnapshot::Capture(clock,balls,"test-v2",ESnapshotPhase::DuringStep);
    CHECK(snapshot.IsValid());
    {
        std::ofstream file("mid-step.snapshot"); CHECK(bool(file)); CHECK(bool(file<<snapshot.Serialize()));
    }
    std::ifstream file("mid-step.snapshot"); CHECK(bool(file));
    const std::string text((std::istreambuf_iterator<char>(file)),{});
    PhysicsSnapshot loaded;
    CHECK(PhysicsSnapshot::Deserialize(text,loaded)); CHECK(snapshot.Serialize()==loaded.Serialize());
    PhysicsClock restoredClock; std::vector<BallState> restoredBalls;
    CHECK(loaded.Restore(restoredClock,restoredBalls,"test-v2"));
    const auto restoredStep=[&](Seconds dt){ restoredBalls[0].PositionMeters+=restoredBalls[0].LinearVelocityMetersPerSecond*dt; };
    for(int i=0;i<600;++i) {
        CHECK(clock.Advance(1.0/60,step)); CHECK(restoredClock.Advance(1.0/60,restoredStep));
    }
    CHECK(balls[0].Serialize()==restoredBalls[0].Serialize());
    CHECK(clock.State().CompletedSteps==restoredClock.State().CompletedSteps);
    CHECK(clock.State().Accumulator==restoredClock.State().Accumulator);
    const auto saved=restoredBalls[0].Serialize(); const auto savedClock=restoredClock.State();
    std::string error;
    CHECK(!loaded.Restore(restoredClock,restoredBalls,"wrong-version",&error)); CHECK(!error.empty());
    CHECK(saved==restoredBalls[0].Serialize()); CHECK(savedClock.CompletedSteps==restoredClock.State().CompletedSteps);
    loaded.AccumulatorSeconds=-1; CHECK(!loaded.Restore(restoredClock,restoredBalls,"test-v2"));
    loaded=snapshot; loaded.SimulationTimeSeconds+=1; CHECK(!loaded.IsValid());
    CHECK(!PhysicsSnapshot::Deserialize("1 BeforeShot test-v1 0 0 0",loaded));
    for(const auto phase:{ESnapshotPhase::BeforeShot,ESnapshotPhase::DuringStep,ESnapshotPhase::AfterShot}) {
        auto value=PhysicsSnapshot::Capture(clock,balls,"test-v2",phase);
        CHECK(PhysicsSnapshot::Deserialize(value.Serialize(),loaded)); CHECK(value==loaded);
    }
    std::cout << "PASS snapshot phase/file/clock restore, unchanged failure and exact continued state\n";
}
