#include <Snooker/Physics/ClothFriction.h>
#include <iostream>
#include <limits>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL friction line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
double Energy(const Snooker::BallState& b)
{
    return 0.5*b.MassKg*b.LinearVelocityMetersPerSecond.LengthSquared()+
           0.2*b.MassKg*b.RadiusMeters*b.RadiusMeters*b.AngularVelocityRadiansPerSecond.LengthSquared();
}
int main()
{
    using namespace Snooker;
    ClothProfile cloth{"cloth-v1","dry",0.2,0,0,0.001,0.01,0.0001,0.25};
    ClothEnvironment env;
    BallState b; b.Id=1; b.Sleeping=false; b.LinearVelocityMetersPerSecond={1,0,0};
    const auto before=Energy(b);
    CHECK(ApplySlidingFriction(b,cloth,env,0.01));
    CHECK(NearlyEqual(b.LinearVelocityMetersPerSecond.X,1-cloth.Sliding*env.GravityMetersPerSecondSquared*0.01));
    CHECK(b.AngularVelocityRadiansPerSecond.Y>0); CHECK(Energy(b)<before);
    for(int i=0;i<1000;++i) {
        const auto energy=Energy(b); CHECK(ApplySlidingFriction(b,cloth,env,1.0/240)); CHECK(Energy(b)<=energy+1e-12);
    }
    const auto slip=b.LinearVelocityMetersPerSecond+b.AngularVelocityRadiansPerSecond.Cross({0,0,-b.RadiusMeters});
    CHECK(slip.Length()<1e-10); CHECK(NearlyEqual(b.LinearVelocityMetersPerSecond.X,5.0/7,1e-9));
    auto tiny=b; tiny.LinearVelocityMetersPerSecond={1e-10,0,0}; tiny.AngularVelocityRadiansPerSecond={};
    CHECK(ApplySlidingFriction(tiny,cloth,env,1)); CHECK(tiny.LinearVelocityMetersPerSecond.X>=0);
    auto zero=cloth; zero.Sliding=0; const auto saved=b.Serialize(); CHECK(ApplySlidingFriction(b,zero,env,1)); CHECK(b.Serialize()==saved);
    CHECK(!ApplySlidingFriction(b,cloth,env,-1)); CHECK(b.Serialize()==saved);
    CHECK(!ApplySlidingFriction(b,cloth,env,std::numeric_limits<double>::quiet_NaN())); CHECK(b.Serialize()==saved);
    std::cout << "PASS sliding friction analytic deceleration, no energy gain, rolling limit and invalid inputs\n";
}
