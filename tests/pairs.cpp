#include <Snooker/Physics/BallPairs.h>
#include <algorithm>
#include <iostream>
#include <limits>
#include <random>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL pairs line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    BallState a,b,c; a.Id=3;b.Id=1;c.Id=2;
    a.PositionMeters={-1,0,0}; a.LinearVelocityMetersPerSecond={1000,0,0};
    b.PositionMeters={0,0,0}; c.PositionMeters={0,10,0};
    std::vector<BallState> balls{a,b,c};std::vector<BallPair> pairs;
    CHECK(FindBallPairCandidates(balls,0.004,pairs)); CHECK(pairs.size()==1); CHECK(pairs[0].First==1 && pairs[0].Second==3);
    const auto expected=pairs;std::reverse(balls.begin(),balls.end());
    CHECK(FindBallPairCandidates(balls,0.004,pairs));CHECK(pairs==expected);
    balls={b}; CHECK(FindBallPairCandidates(balls,0,pairs));CHECK(pairs.empty());
    balls={b,b};pairs=expected;CHECK(!FindBallPairCandidates(balls,1,pairs));CHECK(pairs==expected);
    CHECK(!FindBallPairCandidates({},-1,pairs));CHECK(pairs==expected);
    a.LinearVelocityMetersPerSecond.X=std::numeric_limits<double>::max();balls={a};
    CHECK(!FindBallPairCandidates(balls,2,pairs));CHECK(pairs==expected);
    // Constructed contact oracle independent of the broad-phase implementation.
    std::mt19937 random(90210);std::uniform_real_distribution<double> coordinate(-10,10),velocity(-100,100);
    for(int sample=0;sample<1000;++sample) {
        a.Id=1;b.Id=2;
        a.PositionMeters={coordinate(random),coordinate(random),coordinate(random)};
        a.LinearVelocityMetersPerSecond={velocity(random),velocity(random),velocity(random)};
        b.LinearVelocityMetersPerSecond={velocity(random),velocity(random),velocity(random)};
        const double horizon=0.01, t=horizon*(sample%99+1)/100;
        b.PositionMeters=a.PositionMeters+a.LinearVelocityMetersPerSecond*t+Vec3d{a.RadiusMeters+b.RadiusMeters,0,0}-b.LinearVelocityMetersPerSecond*t;
        CHECK(FindBallPairCandidates({a,b},horizon,pairs));CHECK(pairs.size()==1);
    }
    std::cout<<"PASS swept candidates high-speed contact oracle, stable order and transactional errors\n";
}
