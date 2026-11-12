#include <Snooker/Physics/Sleep.h>
#include <Snooker/Physics/ClothFriction.h>
#include <Snooker/Physics/PhysicsSnapshot.h>
#include <iostream>
#include <limits>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL sleep line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    ClothProfile cloth{"sleep-v1","dry",0.2,0.01,2,0.001,0.01,0.0001,0.25};
    BallState ball; ball.Id=1; ball.Sleeping=false; ball.LinearVelocityMetersPerSecond={1e-5,0,0};
    const Vec3d previous=ball.PositionMeters;
    CHECK(UpdateSleep(ball,previous,cloth,0.1)); CHECK(!ball.Sleeping); CHECK(NearlyEqual(ball.SleepTimeSeconds,0.1));
    BallState restored; CHECK(BallState::Deserialize(ball.Serialize(),restored));
    CHECK(UpdateSleep(restored,previous,cloth,0.1)); CHECK(!restored.Sleeping);
    CHECK(UpdateSleep(restored,previous,cloth,0.05)); CHECK(restored.Sleeping);
    CHECK(restored.LinearVelocityMetersPerSecond.Length()==0); CHECK(restored.AngularVelocityRadiansPerSecond.Length()==0);
    const auto saved=restored.Serialize();
    for(int i=0;i<1000;++i) CHECK(UpdateSleep(restored,previous,cloth,1.0/240));
    CHECK(saved==restored.Serialize());
    restored.LinearVelocityMetersPerSecond={1,0,0}; CHECK(UpdateSleep(restored,previous,cloth,0.01));
    CHECK(!restored.Sleeping); CHECK(restored.SleepTimeSeconds==0);
    restored.LinearVelocityMetersPerSecond={}; restored.AngularVelocityRadiansPerSecond={0,0,1};
    CHECK(UpdateSleep(restored,previous,cloth,1)); CHECK(!restored.Sleeping);
    restored.AngularVelocityRadiansPerSecond={}; restored.PositionMeters.X=1;
    CHECK(UpdateSleep(restored,previous,cloth,1)); CHECK(!restored.Sleeping);
    const auto beforeInvalid=restored.Serialize();
    CHECK(!UpdateSleep(restored,previous,cloth,-1)); CHECK(restored.Serialize()==beforeInvalid);
    CHECK(!UpdateSleep(restored,{std::numeric_limits<double>::quiet_NaN(),0,0},cloth,1)); CHECK(restored.Serialize()==beforeInvalid);
    std::string baseline;
    for(int fps:{30,60,120}) {
        BallState body; body.Id=1; body.Sleeping=false; body.LinearVelocityMetersPerSecond={1,0,0};
        body.AngularVelocityRadiansPerSecond={0,-10,3};
        PhysicsClock clock;
        bool valid=true;
        const auto step=[&](Seconds dt) {
            const auto p=body.PositionMeters;
            valid=ApplyAngularUpdate(body,cloth,{},dt) && ApplyRollingResistance(body,cloth,{},dt) && valid;
            body.PositionMeters+=body.LinearVelocityMetersPerSecond*dt;
            valid=UpdateSleep(body,p,cloth,dt) && valid;
        };
        for(int frame=0;frame<fps*30;++frame) CHECK(clock.Advance(1.0/fps,step));
        CHECK(valid); CHECK(body.Sleeping); CHECK(body.LinearVelocityMetersPerSecond.Length()==0); CHECK(body.AngularVelocityRadiansPerSecond.Length()==0);
        const auto position=body.PositionMeters;
        for(int i=0;i<240;++i) step(1.0/240);
        CHECK(position.NearlyEquals(body.PositionMeters));
        if(baseline.empty()) baseline=body.Serialize(); else CHECK(baseline==body.Serialize());
    }
    std::cout << "PASS sustained sleep, timer persistence, wake, spin/movement guards and same settled state at 30/60/120 FPS\n";
}
