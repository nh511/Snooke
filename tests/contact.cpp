#include <Snooker/Physics/BallContact.h>
#include <Snooker/Physics/BallPairs.h>
#include <iostream>
#include <limits>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL contact line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    BallState a,b; a.Id=1;b.Id=2;a.RadiusMeters=b.RadiusMeters=0.025;
    a.PositionMeters={-1,0,0};a.LinearVelocityMetersPerSecond={1000,0,0};
    BallContactTime hit;
    CHECK(FindBallContactTime(a,b,0.004,hit));CHECK(hit.Hit);CHECK(NearlyEqual(hit.Time,0.00095,1e-14));
    CHECK(hit.Normal.NearlyEquals({1,0,0}));
    const auto contactA=a.PositionMeters+a.LinearVelocityMetersPerSecond*hit.Time;
    CHECK(NearlyEqual((b.PositionMeters-contactA).Length(),0.05,1e-12));
    CHECK((a.PositionMeters+a.LinearVelocityMetersPerSecond*0.004).X>b.PositionMeters.X);
    BallContactTime reversed;CHECK(FindBallContactTime(b,a,0.004,reversed));CHECK(reversed.Hit);
    CHECK(NearlyEqual(reversed.Time,hit.Time));CHECK(reversed.Normal.NearlyEquals(hit.Normal*(-1)));
    CHECK(FindBallContactTime(a,b,0.0009,hit));CHECK(!hit.Hit);
    a.LinearVelocityMetersPerSecond={-1000,0,0};CHECK(FindBallContactTime(a,b,1,hit));CHECK(!hit.Hit);
    a.LinearVelocityMetersPerSecond={1000,0,0};b.PositionMeters.Y=0.05;
    CHECK(FindBallContactTime(a,b,0.004,hit));CHECK(hit.Hit);CHECK(NearlyEqual(hit.Time,0.001,1e-12));
    b.PositionMeters.Y=0.051;CHECK(FindBallContactTime(a,b,0.004,hit));CHECK(!hit.Hit);
    a.PositionMeters={0,0,0};b.PositionMeters={0.05,0,0};
    CHECK(FindBallContactTime(a,b,0,hit));CHECK(hit.Hit);CHECK(hit.Time==0);CHECK(!hit.InitiallyOverlapping);
    b.PositionMeters={};CHECK(FindBallContactTime(a,b,0,hit));CHECK(hit.Hit && hit.InitiallyOverlapping);CHECK(hit.Normal.IsFinite());
    const auto saved=hit;CHECK(!FindBallContactTime(a,b,-1,hit));CHECK(hit.Hit==saved.Hit && hit.Time==saved.Time);
    CHECK(!FindBallContactTime(a,a,1,hit));
    CHECK(!FindBallContactTime(a,b,std::numeric_limits<double>::quiet_NaN(),hit));
    // Analytic contact family across speeds and unequal radii, checked after broad phase.
    for(int i=1;i<=500;++i) {
        a.RadiusMeters=0.02;b.RadiusMeters=0.03;
        const double speed=i*0.1, time=0.001*i;
        a.PositionMeters={-0.05-speed*time,0,0};b.PositionMeters={};
        a.LinearVelocityMetersPerSecond={speed,0,0};b.LinearVelocityMetersPerSecond={};
        std::vector<BallPair> pairs;CHECK(FindBallPairCandidates({a,b},time*2,pairs));CHECK(pairs.size()==1);
        CHECK(FindBallContactTime(a,b,time*2,hit));CHECK(hit.Hit);CHECK(NearlyEqual(hit.Time,time,1e-9));
    }
    std::cout<<"PASS continuous sphere contact: high speed, tangency, misses, overlap and analytic family\n";
}
