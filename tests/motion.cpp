#include <Snooker/Physics/MotionState.h>
#include <Snooker/Physics/ClothFriction.h>
#include <iostream>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL motion line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    BallState ball; ball.Id=1; ball.Sleeping=false; ball.LinearVelocityMetersPerSecond={4,0,0};
    ClothMotion mode=ClothMotion::Sliding;
    MotionThresholds thresholds;
    ClothProfile cloth{"motion-v1","dry",0.2,0.01,0.1,0.001,0.01,0.0001,0.25};
    CHECK(ClassifyClothMotion(ball,mode,thresholds,mode)); CHECK(mode==ClothMotion::Sliding);
    CHECK(ApplyAngularUpdate(ball,cloth,{},1.0/240));
    CHECK(ClassifyClothMotion(ball,mode,thresholds,mode)); CHECK(mode==ClothMotion::Sliding);
    unsigned changes=0;
    for(int i=0;i<2000;++i) {
        CHECK(ApplyAngularUpdate(ball,cloth,{},1.0/240));
        const auto previous=mode;
        CHECK(ClassifyClothMotion(ball,mode,thresholds,mode)); if(previous!=mode) ++changes;
    }
    CHECK(mode==ClothMotion::Rolling); CHECK(changes==1);
    ball.AngularVelocityRadiansPerSecond={};
    for(int i=0;i<1000;++i) {
        ball.LinearVelocityMetersPerSecond={i%2?2e-8:9e-7,0,0};
        const auto saved=ball.Serialize();
        CHECK(ClassifyClothMotion(ball,mode,thresholds,mode)); CHECK(mode==ClothMotion::Rolling);
        CHECK(saved==ball.Serialize());
    }
    ball.LinearVelocityMetersPerSecond={1e-5,0,0};
    CHECK(ClassifyClothMotion(ball,mode,thresholds,mode)); CHECK(mode==ClothMotion::Sliding);
    thresholds.ExitRollingSlip=thresholds.EnterRollingSlip;
    CHECK(!ClassifyClothMotion(ball,mode,thresholds,mode)); CHECK(mode==ClothMotion::Sliding);
    std::cout << "PASS gradual slide-to-roll, single transition, low-slip hysteresis and read-only classification\n";
}
