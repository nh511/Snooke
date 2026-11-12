#include <Snooker/Physics/BallResponse.h>
#include <algorithm>
#include <iomanip>
#include <locale>
#include <sstream>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) { if(Error)*Error=Message;return false; }
bool Token(const std::string& v) { return !v.empty() && v.find_first_of(" \t\r\n")==std::string::npos; }
}
bool BallCollisionProfile::IsValid(std::string* Error) const
{
    if(!Token(ParameterVersion) || !IsFinite(Restitution) || Restitution<0 || Restitution>1 ||
       !IsFinite(TangentialFriction) || TangentialFriction<0 || !IsFinite(ContactTolerance) || ContactTolerance<=0)
        return Fail(Error,"invalid ball collision profile");
    return true;
}
std::string BallCollisionProfile::Serialize() const
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(std::numeric_limits<Scalar>::max_digits10)
        <<"snooker-ball-contact-v1 "<<ParameterVersion<<' '<<Restitution<<' '<<TangentialFriction<<' '<<ContactTolerance;
    return s.str();
}
bool BallCollisionProfile::Deserialize(const std::string& Text,BallCollisionProfile& Output,std::string* Error)
{
    std::istringstream s(Text);s.imbue(std::locale::classic());BallCollisionProfile p;std::string magic;
    if(!(s>>magic>>p.ParameterVersion>>p.Restitution>>p.TangentialFriction>>p.ContactTolerance) || magic!="snooker-ball-contact-v1")
        return Fail(Error,"invalid ball collision schema");
    s>>std::ws;
    if(!s.eof())return Fail(Error,"unexpected ball collision profile tail");
    if(!p.IsValid(Error))return false;
    Output=std::move(p);return true;
}
Scalar BallKineticEnergy(const BallState& b)
{
    return 0.5*b.MassKg*b.LinearVelocityMetersPerSecond.LengthSquared()+
           0.2*b.MassKg*b.RadiusMeters*b.RadiusMeters*b.AngularVelocityRadiansPerSecond.LengthSquared();
}
bool ResolveBallContact(BallState& First,BallState& Second,const BallCollisionProfile& Profile,
                        BallCollisionResponse& Output,std::string* Error)
{
    if(!First.IsValid(Error) || !Second.IsValid(Error) || !Profile.IsValid(Error))return false;
    if(&First==&Second || First.Id==Second.Id)return Fail(Error,"collision requires distinct ball ids");
    const Vec3d separation=Second.PositionMeters-First.PositionMeters;
    const Scalar distance=std::hypot(separation.X,separation.Y,separation.Z), radius=First.RadiusMeters+Second.RadiusMeters;
    if(!separation.IsFinite() || !IsFinite(distance) || !IsFinite(radius) || distance<=0 ||
       std::abs(distance-radius)>Profile.ContactTolerance ||
       Profile.ContactTolerance>=std::min(First.RadiusMeters,Second.RadiusMeters))
        return Fail(Error,"balls not at valid contact; overlap correction is not implemented");
    BallCollisionResponse r;r.Normal=separation*(1/distance);
    const Vec3d leverA=r.Normal*First.RadiusMeters, leverB=r.Normal*(-Second.RadiusMeters);
    r.Point=(First.PositionMeters+leverA+Second.PositionMeters+leverB)*0.5;
    r.RelativeContactVelocity=Second.LinearVelocityMetersPerSecond+Second.AngularVelocityRadiansPerSecond.Cross(leverB)
        -First.LinearVelocityMetersPerSecond-First.AngularVelocityRadiansPerSecond.Cross(leverA);
    r.EnergyBefore=BallKineticEnergy(First)+BallKineticEnergy(Second);r.EnergyAfter=r.EnergyBefore;
    const Scalar normalSpeed=r.RelativeContactVelocity.Dot(r.Normal);
    if(!r.Point.IsFinite() || !r.RelativeContactVelocity.IsFinite() || !IsFinite(normalSpeed) || !IsFinite(r.EnergyBefore))
        return Fail(Error,"collision state arithmetic overflow");
    if(normalSpeed>=0) {Output=r;return true;}
    const Scalar inertiaA=0.4*First.MassKg*First.RadiusMeters*First.RadiusMeters;
    const Scalar inertiaB=0.4*Second.MassKg*Second.RadiusMeters*Second.RadiusMeters;
    const Scalar inverseMass=1/First.MassKg+1/Second.MassKg;
    if(!IsFinite(inertiaA) || !IsFinite(inertiaB) || inertiaA<=0 || inertiaB<=0 || !IsFinite(inverseMass))
        return Fail(Error,"invalid collision effective mass");
    r.NormalImpulse=-(1+Profile.Restitution)*normalSpeed/inverseMass;
    const Vec3d tangent=r.RelativeContactVelocity-r.Normal*normalSpeed;
    const Scalar tangentSpeed=std::hypot(tangent.X,tangent.Y,tangent.Z);
    const Scalar tangentInverse=inverseMass+First.RadiusMeters*First.RadiusMeters/inertiaA+Second.RadiusMeters*Second.RadiusMeters/inertiaB;
    if(!IsFinite(tangentSpeed) || !IsFinite(tangentInverse) || !IsFinite(r.NormalImpulse))return Fail(Error,"collision impulse overflow");
    if(tangentSpeed>0 && Profile.TangentialFriction>0) {
        const Scalar frictionLimit=Profile.TangentialFriction*r.NormalImpulse;
        if(!IsFinite(frictionLimit))return Fail(Error,"collision friction overflow");
        const Scalar magnitude=std::min(tangentSpeed/tangentInverse,frictionLimit);
        r.TangentialImpulse=tangent*(-magnitude/tangentSpeed);
    }
    const Vec3d impulse=r.Normal*r.NormalImpulse+r.TangentialImpulse;
    auto a=First,b=Second;
    a.LinearVelocityMetersPerSecond+=impulse*(-1/a.MassKg);
    b.LinearVelocityMetersPerSecond+=impulse*(1/b.MassKg);
    a.AngularVelocityRadiansPerSecond+=leverA.Cross(impulse)*(-1/inertiaA);
    b.AngularVelocityRadiansPerSecond+=leverB.Cross(impulse)*(1/inertiaB);
    a.Sleeping=b.Sleeping=false;a.SleepTimeSeconds=b.SleepTimeSeconds=0;
    a.Contact={EContactKind::Ball,b.Id,r.Normal};b.Contact={EContactKind::Ball,a.Id,r.Normal*(-1)};
    if(!a.IsValid(Error) || !b.IsValid(Error))return false;
    r.EnergyAfter=BallKineticEnergy(a)+BallKineticEnergy(b);
    if(!IsFinite(r.EnergyAfter) || r.EnergyAfter>r.EnergyBefore+1e-10*std::max(1.0,r.EnergyBefore))
        return Fail(Error,"collision energy increased beyond numerical tolerance");
    r.Applied=true;First=a;Second=b;Output=r;return true;
}
}
