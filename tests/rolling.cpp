#include <Snooker/Physics/ClothFriction.h>
#include <iostream>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL rolling line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
double Energy(const Snooker::BallState& b)
{
    return 0.5*b.MassKg*b.LinearVelocityMetersPerSecond.LengthSquared()+
           0.2*b.MassKg*b.RadiusMeters*b.RadiusMeters*b.AngularVelocityRadiansPerSecond.LengthSquared();
}
int main()
{
    using namespace Snooker;
    ClothEnvironment env;
    ClothProfile cloth{"cloth-v1","dry",0.2,0.01,0,0.001,0.01,0.0001,0.25};
    double distances[2]{};
    for(int preset=0;preset<2;++preset) {
        auto profile=cloth; profile.Rolling*=preset+1;
        BallState b; b.Id=1; b.Sleeping=false; b.LinearVelocityMetersPerSecond={1,0,0};
        b.AngularVelocityRadiansPerSecond={0,1/b.RadiusMeters,0};
        for(int i=0;i<10000;++i) {
            const auto previous=Energy(b);
            CHECK(ApplyRollingResistance(b,profile,env,1.0/240));
            CHECK(Energy(b)<=previous+1e-12); CHECK(b.LinearVelocityMetersPerSecond.X>=0);
            b.PositionMeters+=b.LinearVelocityMetersPerSecond*(1.0/240);
        }
        CHECK(b.LinearVelocityMetersPerSecond.Length()==0); CHECK(b.AngularVelocityRadiansPerSecond.Length()==0);
        distances[preset]=b.PositionMeters.X;
        CHECK(std::abs(distances[preset]-1/(2*profile.Rolling*env.GravityMetersPerSecondSquared))<0.005);
    }
    CHECK(distances[0]>distances[1]);
    BallState sliding; sliding.Id=1; sliding.Sleeping=false; sliding.LinearVelocityMetersPerSecond={1,0,0};
    auto preserved=sliding.Serialize(); CHECK(ApplyRollingResistance(sliding,cloth,env,1)); CHECK(sliding.Serialize()==preserved);
    for(int i=0;i<10000;++i) {
        const auto previous=Energy(sliding);
        CHECK(ApplySlidingFriction(sliding,cloth,env,1.0/240)); CHECK(ApplyRollingResistance(sliding,cloth,env,1.0/240));
        CHECK(Energy(sliding)<=previous+1e-12);
        sliding.PositionMeters+=sliding.LinearVelocityMetersPerSecond*(1.0/240);
    }
    CHECK(sliding.LinearVelocityMetersPerSecond.Length()==0); CHECK(sliding.AngularVelocityRadiansPerSecond.Length()==0);
    preserved=sliding.Serialize(); CHECK(!ApplyRollingResistance(sliding,cloth,env,-1)); CHECK(sliding.Serialize()==preserved);
    std::cout << "PASS rolling distance monotonicity, analytical stopping distance and combined slide-to-stop\n";
}
