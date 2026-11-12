#include <Snooker/Physics/ClothFriction.h>
#include <iostream>
#include <limits>

#define CHECK(x) do { if (!(x)) { std::cerr << "FAIL angular line " << __LINE__ << ": " << #x << '\n'; return 1; } } while(false)
int main()
{
    using namespace Snooker;
    ClothProfile cloth{"spin-v1","dry",0.2,0.01,2,0.001,0.01,0.0001,0.25};
    ClothEnvironment env;
    BallState base; base.Id=1; base.Sleeping=false; base.LinearVelocityMetersPerSecond={1,0,0};
    auto back=base, follow=base;
    back.AngularVelocityRadiansPerSecond={0,-20,0};
    follow.AngularVelocityRadiansPerSecond={0,60,0};
    CHECK(ApplyAngularUpdate(back,cloth,env,0.01)); CHECK(ApplyAngularUpdate(follow,cloth,env,0.01));
    CHECK(back.AngularVelocityRadiansPerSecond.Y>-20); CHECK(back.LinearVelocityMetersPerSecond.X<1);
    CHECK(follow.AngularVelocityRadiansPerSecond.Y<60); CHECK(follow.LinearVelocityMetersPerSecond.X>1);
    auto left=base,right=base;
    left.LinearVelocityMetersPerSecond={}; right.LinearVelocityMetersPerSecond={};
    left.AngularVelocityRadiansPerSecond={0,0,3}; right.AngularVelocityRadiansPerSecond={0,0,-3};
    for(int i=0;i<1000;++i) {
        const double previous=std::abs(left.AngularVelocityRadiansPerSecond.Z);
        CHECK(ApplyAngularUpdate(left,cloth,env,1.0/240)); CHECK(ApplyAngularUpdate(right,cloth,env,1.0/240));
        CHECK(std::abs(left.AngularVelocityRadiansPerSecond.Z)<=previous);
        CHECK(NearlyEqual(left.AngularVelocityRadiansPerSecond.Z,-right.AngularVelocityRadiansPerSecond.Z));
        CHECK(left.LinearVelocityMetersPerSecond.Length()==0); CHECK(right.LinearVelocityMetersPerSecond.Length()==0);
    }
    CHECK(left.AngularVelocityRadiansPerSecond.Length()==0); CHECK(right.AngularVelocityRadiansPerSecond.Length()==0);
    auto composed=base, reference=base;
    composed.AngularVelocityRadiansPerSecond.Z=1; reference=composed;
    CHECK(ApplyAngularUpdate(composed,cloth,env,0.25)); CHECK(ApplySlidingFriction(reference,cloth,env,0.25));
    CHECK(composed.LinearVelocityMetersPerSecond.NearlyEquals(reference.LinearVelocityMetersPerSecond));
    CHECK(NearlyEqual(composed.AngularVelocityRadiansPerSecond.Y,reference.AngularVelocityRadiansPerSecond.Y));
    CHECK(NearlyEqual(composed.AngularVelocityRadiansPerSecond.Z,0.5));
    const auto saved=composed.Serialize();
    CHECK(!ApplyAngularUpdate(composed,cloth,env,std::numeric_limits<double>::quiet_NaN())); CHECK(composed.Serialize()==saved);
    auto sleeping=composed; sleeping.Sleeping=true; const auto asleep=sleeping.Serialize();
    CHECK(ApplyAngularUpdate(sleeping,cloth,env,1)); CHECK(sleeping.Serialize()==asleep);
    std::cout << "PASS follow/backspin contact torque, mirror side spin, damping and single impulse composition\n";
}
