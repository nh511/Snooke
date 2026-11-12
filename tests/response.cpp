#include <Snooker/Physics/BallResponse.h>
#include <Snooker/Physics/BallContact.h>
#include <fstream>
#include <iostream>
#include <iterator>

#define CHECK(x) do { if(!(x)) { std::cerr<<"FAIL response line "<<__LINE__<<": "<<#x<<'\n'; return 1; } } while(false)
int main(int argc,char** argv)
{
    using namespace Snooker;CHECK(argc==2);
    std::ifstream file(argv[1]);CHECK(bool(file));const std::string text((std::istreambuf_iterator<char>(file)),{});
    BallCollisionProfile material;CHECK(BallCollisionProfile::Deserialize(text,material));
    BallCollisionProfile loaded;CHECK(BallCollisionProfile::Deserialize(material.Serialize(),loaded));CHECK(loaded.Serialize()==material.Serialize());
    const auto savedMaterial=loaded.Serialize();CHECK(!BallCollisionProfile::Deserialize(text+"junk",loaded));CHECK(savedMaterial==loaded.Serialize());
    BallState first,second;first.Id=1;second.Id=2;first.Sleeping=false;
    first.PositionMeters={-1,0,0};first.LinearVelocityMetersPerSecond={1000,0,0};
    BallContactTime toi;CHECK(FindBallContactTime(first,second,0.004,toi));CHECK(toi.Hit);
    first.PositionMeters+=first.LinearVelocityMetersPerSecond*toi.Time;
    auto elastic=material;elastic.Restitution=1;elastic.TangentialFriction=0;
    BallCollisionResponse result;CHECK(ResolveBallContact(first,second,elastic,result));CHECK(result.Applied);
    CHECK(NearlyEqual(first.LinearVelocityMetersPerSecond.X,0,1e-9));CHECK(NearlyEqual(second.LinearVelocityMetersPerSecond.X,1000,1e-9));
    CHECK(NearlyEqual(result.EnergyBefore,result.EnergyAfter,1e-6));CHECK(!second.Sleeping);
    first.PositionMeters+=first.LinearVelocityMetersPerSecond*(0.004-toi.Time);
    second.PositionMeters+=second.LinearVelocityMetersPerSecond*(0.004-toi.Time);
    CHECK((second.PositionMeters-first.PositionMeters).Length()>=first.RadiusMeters+second.RadiusMeters);
    for(const double speed:{1e-8,0.01,1.0,10.0}) for(const double fraction:{0.0,0.5,0.99}) {
        BallState a,b;a.Id=1;b.Id=2;a.Sleeping=false;
        const Vec3d normal{std::sqrt(1-fraction*fraction),fraction,0};
        b.PositionMeters=normal*(a.RadiusMeters+b.RadiusMeters);
        a.LinearVelocityMetersPerSecond={speed,0,0};a.AngularVelocityRadiansPerSecond={0,0,speed*10};
        const auto momentum=a.LinearVelocityMetersPerSecond*a.MassKg+b.LinearVelocityMetersPerSecond*b.MassKg;
        const auto angularBefore=a.PositionMeters.Cross(a.LinearVelocityMetersPerSecond*a.MassKg)+b.PositionMeters.Cross(b.LinearVelocityMetersPerSecond*b.MassKg)+a.AngularVelocityRadiansPerSecond*(0.4*a.MassKg*a.RadiusMeters*a.RadiusMeters);
        CHECK(ResolveBallContact(a,b,material,result));CHECK(result.Applied);CHECK(result.EnergyAfter<=result.EnergyBefore+1e-12);
        CHECK((a.LinearVelocityMetersPerSecond*a.MassKg+b.LinearVelocityMetersPerSecond*b.MassKg).NearlyEquals(momentum,1e-10));
        const auto angularAfter=a.PositionMeters.Cross(a.LinearVelocityMetersPerSecond*a.MassKg)+b.PositionMeters.Cross(b.LinearVelocityMetersPerSecond*b.MassKg)+a.AngularVelocityRadiansPerSecond*(0.4*a.MassKg*a.RadiusMeters*a.RadiusMeters)+b.AngularVelocityRadiansPerSecond*(0.4*b.MassKg*b.RadiusMeters*b.RadiusMeters);
        CHECK(angularAfter.NearlyEquals(angularBefore,1e-10));
        const auto separatingA=a.Serialize(),separatingB=b.Serialize();
        CHECK(ResolveBallContact(a,b,material,result));CHECK(!result.Applied);CHECK(a.Serialize()==separatingA && b.Serialize()==separatingB);
    }
    BallState a,b;a.Id=1;b.Id=2;b.PositionMeters={a.RadiusMeters+b.RadiusMeters,0,0};
    a.Sleeping=false;a.LinearVelocityMetersPerSecond={1,0,0};b.MassKg=a.MassKg*2;
    CHECK(ResolveBallContact(a,b,elastic,result));CHECK(NearlyEqual(a.LinearVelocityMetersPerSecond.X,-1.0/3));CHECK(NearlyEqual(b.LinearVelocityMetersPerSecond.X,2.0/3));
    const auto savedA=a.Serialize(),savedB=b.Serialize();result.NormalImpulse=77;
    auto bad=material;bad.Restitution=2;CHECK(!ResolveBallContact(a,b,bad,result));CHECK(result.NormalImpulse==77);CHECK(a.Serialize()==savedA && b.Serialize()==savedB);
    b.PositionMeters={};CHECK(!ResolveBallContact(a,b,material,result));CHECK(result.NormalImpulse==77);
    std::cout<<"PASS sphere response: high-speed split, head/thin/slow/spin, momentum/angular momentum/energy and invalid state\n";
}
