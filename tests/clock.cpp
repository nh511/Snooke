#include <Snooker/Physics/PhysicsClock.h>
#include <Snooker/Physics/BallState.h>
#include <iostream>
#include <limits>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL clock line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    std::string Baseline;
    for (const int rate:{30,60,120}) {
        BallState ball; ball.Id=1; ball.Sleeping=false; ball.LinearVelocityMetersPerSecond={0.5,0.25,0};
        PhysicsClock clock;
        const auto step=[&](Seconds dt) { ball.PositionMeters+=ball.LinearVelocityMetersPerSecond*dt; };
        for(int frame=0;frame<rate*10;++frame) CHECK(clock.Advance(1.0/rate,step));
        CHECK(clock.State().CompletedSteps==2400);
        CHECK(NearlyEqual(clock.SimulationTime(),10)); CHECK(NearlyEqual(clock.State().Accumulator,0));
        CHECK(ball.PositionMeters.NearlyEquals({5,2.5,0}));
        if (Baseline.empty()) Baseline=ball.Serialize(); else CHECK(Baseline==ball.Serialize());
    }
    PhysicsClock clock;
    unsigned calls=0;
    const auto step=[&](Seconds dt) { if(dt>0) ++calls; };
    CHECK(clock.Advance(1.0/480,step)); CHECK(calls==0);
    const auto saved=clock.State(); PhysicsClock resumed; CHECK(resumed.Restore(saved));
    CHECK(resumed.Advance(1.0/480,step)); CHECK(calls==1);
    clock=PhysicsClock{}; calls=0;
    CHECK(clock.Advance(1,step,8)); CHECK(calls==8); CHECK(clock.State().Accumulator>0.9);
    for(int i=0;i<29;++i) CHECK(clock.Advance(0,step,8));
    CHECK(calls==240); CHECK(NearlyEqual(clock.State().Accumulator,0));
    const auto before=clock.State();
    CHECK(!clock.Advance(-1,step)); CHECK(!clock.Advance(std::numeric_limits<double>::quiet_NaN(),step));
    CHECK(!clock.Advance(1,step,0)); CHECK(!clock.Advance(4000,step)); CHECK(!clock.Advance(1,{}));
    CHECK(clock.State().CompletedSteps==before.CompletedSteps); CHECK(clock.State().Accumulator==before.Accumulator);
    auto invalid=before; invalid.FixedStep=0; CHECK(!clock.Restore(invalid));
    for(int frequency:{120,240,480}) {
        ClockState state; state.FixedStep=1.0/frequency; CHECK(clock.Restore(state)); calls=0;
        for(int i=0;i<60;++i) CHECK(clock.Advance(1.0/60,step));
        CHECK(calls==static_cast<unsigned>(frequency));
    }
    std::cout << "PASS fixed clock: 30/60/120 FPS identical BallState, 120/240/480 Hz, debt, restore and invalid time\n";
}
