#include <Snooker/Physics/BallContact.h>
#include <algorithm>

namespace Snooker
{
bool FindBallContactTime(const BallState& First,const BallState& Second,Seconds Horizon,
                          BallContactTime& Output,std::string* Error)
{
    const auto fail=[&](const char* message){if(Error)*Error=message;return false;};
    if(!First.IsValid(Error) || !Second.IsValid(Error)) return false;
    if(First.Id==Second.Id || !IsFinite(Horizon) || Horizon<0) return fail("invalid contact ids or horizon");
    const Vec3d separation=Second.PositionMeters-First.PositionMeters;
    const Vec3d relative=Second.LinearVelocityMetersPerSecond-First.LinearVelocityMetersPerSecond;
    const Scalar radius=First.RadiusMeters+Second.RadiusMeters;
    const Scalar distance=std::hypot(separation.X,separation.Y,separation.Z);
    if(!separation.IsFinite() || !relative.IsFinite() || !IsFinite(radius) || !IsFinite(distance))
        return fail("relative contact geometry overflow");
    BallContactTime result;
    const auto normal=[&](const Vec3d& p,Vec3d& n) {
        const Scalar length=std::hypot(p.X,p.Y,p.Z);
        if(!IsFinite(length)) return false;
        n=length>0?p*(1/length):Vec3d{First.Id<Second.Id?1.0:-1.0,0,0};
        return n.IsFinite();
    };
    if(distance<=radius) {
        result.Hit=true;result.InitiallyOverlapping=distance<radius;
        if(!normal(separation,result.Normal)) return fail("invalid initial normal");
        Output=result;return true;
    }
    if(Horizon==0) {Output=result;return true;}
    const Vec3d travel=relative*Horizon;
    if(!travel.IsFinite()) return fail("relative contact travel overflow");
    // Scale distance/travel together, solve dimensionless u=t/Horizon in [0,1].
    const Scalar scale=std::max({std::abs(separation.X),std::abs(separation.Y),std::abs(separation.Z),
                                std::abs(travel.X),std::abs(travel.Y),std::abs(travel.Z),radius});
    const Vec3d p{separation.X/scale,separation.Y/scale,separation.Z/scale};
    const Vec3d d{travel.X/scale,travel.Y/scale,travel.Z/scale};
    const Scalar r=radius/scale, a=d.Dot(d), b=p.Dot(d), c=p.Dot(p)-r*r;
    if(a==0 || b>=0) {Output=result;return true;}
    const Scalar discriminant=b*b-a*c;
    if(discriminant<0) {Output=result;return true;}
    const Scalar denominator=-b+std::sqrt(discriminant);
    if(denominator<=0) {Output=result;return true;}
    // c/(-b+sqrt(D)) avoids cancellation in the early root.
    const Scalar u=c/denominator;
    if(!IsFinite(u)) return fail("contact root overflow");
    if(u<0 || u>1) {Output=result;return true;}
    result.Hit=true;result.Time=u*Horizon;
    if(!normal(p+d*u,result.Normal)) return fail("invalid contact normal");
    Output=result;return true;
}
}
