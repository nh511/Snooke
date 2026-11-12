#include <Snooker/Physics/PocketContact.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace Snooker
{
namespace
{
double Length(const Vec3d& v) {return std::hypot(v.X,v.Y,v.Z);}
bool Fail(std::string* Error,const char* Message) {if(Error)*Error=Message;return false;}
// A bounded arithmetic envelope rejects unrepresentable trajectories explicitly.
bool Safe(const Vec3d& v) {return v.IsFinite() && std::max({std::abs(v.X),std::abs(v.Y),std::abs(v.Z)})<=1e6;}
bool Unit(const Vec3d& v) {return Safe(v) && NearlyEqual(Length(v),1,1e-10);}
bool Validate(const PocketGeometry& p,std::string* Error)
{
    if(!p.Parameters.IsValid(Error))return false;
    if(!p.Definition.Id || p.Definition.ProfileId!=p.Parameters.Id ||
       !NearlyEqual(p.Definition.Opening,p.Parameters.OpeningWidth,1e-10) ||
       !NearlyEqual(p.Definition.Depth,p.Parameters.BottomDepth,1e-10) ||
       !Safe(p.Definition.Center) || !Unit(p.Inward) || !Unit(p.Tangent) ||
       std::abs(p.Inward.Z)>1e-10 || std::abs(p.Tangent.Z)>1e-10 ||
       std::abs(p.Inward.Dot(p.Tangent))>1e-10 || p.Floor.size()!=4 || p.Bottom.size()!=4 ||
       p.LeftJaw.size()<2 || p.RightJaw.size()<2 || p.LeftJaw.size()>257 || p.RightJaw.size()>257)
        return Fail(Error,"invalid pocket geometry/basis/profile association");
    for(const auto* vertices:{&p.LeftJaw,&p.RightJaw,&p.Floor,&p.Bottom})
        for(const auto& v:*vertices)if(!Safe(v))return Fail(Error,"unsafe pocket vertex");
    if(!Safe(p.MouthStart) || !Safe(p.MouthEnd) || !Safe(p.BottomCenter) ||
       Length(p.MouthEnd-p.MouthStart)<=1e-12)return Fail(Error,"invalid pocket mouth");
    for(const auto* vertices:{&p.Floor,&p.Bottom}) {
        if(Length(((*vertices)[1]-(*vertices)[0]).Cross((*vertices)[2]-(*vertices)[0]))<=1e-15 ||
           Length(((*vertices)[2]-(*vertices)[0]).Cross((*vertices)[3]-(*vertices)[0]))<=1e-15)
            return Fail(Error,"degenerate pocket surface");
    }
    return true;
}
// Earliest nonnegative root of |p+d*u|=radius, in normalized u=t/horizon.
bool Root(const Vec3d& p,const Vec3d& d,double radius,double& u)
{
    const double scale=std::max({Length(p),Length(d),radius});
    const auto a=p*(1/scale),b=d*(1/scale);const double r=radius/scale;
    const double aa=b.Dot(b),bb=a.Dot(b),cc=a.Dot(a)-r*r;
    if(aa==0 || bb>=0)return false;
    double disc=bb*bb-aa*cc;
    const double roundoff=32*std::numeric_limits<double>::epsilon()*(bb*bb+std::abs(aa*cc));
    if(disc < -roundoff)return false;
    disc=std::max(0.0,disc);
    const double denominator=-bb+std::sqrt(disc);
    if(denominator<=0)return false;
    u=cc/denominator;
    return IsFinite(u) && u>=0 && u<=1;
}
struct Sweep
{
    const BallState& Ball;
    const PocketGeometry& Pocket;
    Seconds Horizon;
    PocketContact Best;
    std::size_t Primitive=0;
    void Offer(double u,const Vec3d& point,const Vec3d& fallback,PocketFeature feature)
    {
        const Vec3d center=Ball.PositionMeters+Ball.LinearVelocityMetersPerSecond*(u*Horizon);
        const Vec3d offset=center-point;const double distance=Length(offset);
        if(!Best.Hit || u*Horizon<Best.Time) {
            Best.Hit=true;Best.Time=u*Horizon;Best.PocketId=Pocket.Definition.Id;Best.Feature=feature;
            Best.Primitive=Primitive;Best.Point=point;Best.Normal=distance>1e-14?offset*(1/distance):fallback;
            Best.InitiallyOverlapping=u==0 && distance<Ball.RadiusMeters-1e-10;
        }
    }
    void Segment(const Vec3d& a,const Vec3d& b,PocketFeature feature)
    {
        ++Primitive;
        const Vec3d edge=b-a;const double length=Length(edge);
        const auto travel=Ball.LinearVelocityMetersPerSecond*Horizon;
        if(length<=1e-14) {Point(a,feature);return;}
        const Vec3d t=edge*(1/length),from=Ball.PositionMeters-a;
        const auto nearest=a+t*std::clamp(from.Dot(t),0.0,length);
        if(Length(Ball.PositionMeters-nearest)<=Ball.RadiusMeters)Offer(0,nearest,Pocket.Inward,feature);
        double u=0;
        const Vec3d perpendicular=from-t*from.Dot(t),velocity=travel-t*travel.Dot(t);
        if(Root(perpendicular,velocity,Ball.RadiusMeters,u)) {
            const double along=(from+travel*u).Dot(t);
            if(along>=0 && along<=length)Offer(u,a+t*along,Pocket.Inward,feature);
        }
        Point(a,feature);Point(b,feature);
    }
    void Point(const Vec3d& point,PocketFeature feature)
    {
        const auto from=Ball.PositionMeters-point;
        if(Length(from)<=Ball.RadiusMeters)Offer(0,point,Pocket.Inward,feature);
        double u=0;
        if(Root(from,Ball.LinearVelocityMetersPerSecond*Horizon,Ball.RadiusMeters,u))Offer(u,point,Pocket.Inward,feature);
    }
    void Triangle(const Vec3d& a,const Vec3d& b,const Vec3d& c,PocketFeature feature)
    {
        ++Primitive;
        const auto cross=(b-a).Cross(c-a);const auto n=cross*(1/Length(cross));
        const auto inside=[&](const Vec3d& p) {
            const double ab=(b-a).Cross(p-a).Dot(n),bc=(c-b).Cross(p-b).Dot(n),ca=(a-c).Cross(p-c).Dot(n);
            return ab>=0 && bc>=0 && ca>=0;
        };
        const double height=(Ball.PositionMeters-a).Dot(n);
        const Vec3d projection=Ball.PositionMeters-n*height;
        if(std::abs(height)<=Ball.RadiusMeters && inside(projection))Offer(0,projection,n,feature);
        const double change=Ball.LinearVelocityMetersPerSecond.Dot(n)*Horizon;
        if(change!=0)for(double side:{-1.0,1.0}) {
            const double u=(side*Ball.RadiusMeters-height)/change;
            if(u<0 || u>1)continue;
            const Vec3d point=Ball.PositionMeters+Ball.LinearVelocityMetersPerSecond*(u*Horizon)-n*(side*Ball.RadiusMeters);
            if(inside(point))Offer(u,point,n*side,feature);
        }
        Segment(a,b,feature);Segment(b,c,feature);Segment(c,a,feature);
    }
};
}
bool FindPocketContact(const BallState& Ball,const PocketGeometry& Pocket,Seconds Horizon,
                       PocketContact& Output,std::string* Error)
{
    if(!Ball.IsValid(Error) || !Validate(Pocket,Error))return false;
    if(!IsFinite(Horizon) || Horizon<0 || !Safe(Ball.PositionMeters) || !Safe(Ball.LinearVelocityMetersPerSecond) ||
       Ball.RadiusMeters<1e-9 || Ball.RadiusMeters>1e3 || !Safe(Ball.LinearVelocityMetersPerSecond*Horizon) ||
       !Safe(Ball.PositionMeters+Ball.LinearVelocityMetersPerSecond*Horizon))
        return Fail(Error,"unsafe pocket sweep bounds or horizon");
    Sweep sweep{Ball,Pocket,Horizon,{}};
    for(const auto* jaw:{&Pocket.LeftJaw,&Pocket.RightJaw})
        for(std::size_t i=1;i<jaw->size();++i)sweep.Segment((*jaw)[i-1],(*jaw)[i],PocketFeature::Jaw);
    // Mouth is an opening, not a solid segment across the bag. Only its side lips are solid.
    sweep.Segment(Pocket.Floor[0],Pocket.Floor[3],PocketFeature::Rim);
    sweep.Segment(Pocket.Floor[1],Pocket.Floor[2],PocketFeature::Rim);
    for(const auto* surface:{&Pocket.Floor,&Pocket.Bottom}) {
        const auto feature=surface==&Pocket.Floor?PocketFeature::Slope:PocketFeature::Bottom;
        sweep.Triangle((*surface)[0],(*surface)[1],(*surface)[2],feature);
        sweep.Triangle((*surface)[0],(*surface)[2],(*surface)[3],feature);
    }
    if(sweep.Best.Hit && (!sweep.Best.Point.IsFinite() || !Unit(sweep.Best.Normal)))
        return Fail(Error,"nonfinite pocket sweep result");
    Output=sweep.Best;return true;
}
bool ReflectPocketContact(BallState& Ball,const PocketGeometry& Pocket,const PocketContact& Contact,std::string* Error)
{
    PocketContact actual;
    if(!FindPocketContact(Ball,Pocket,0,actual,Error))return false;
    // Query at t=0 may differ by roundoff at an exact analytical boundary.
    if(!Contact.Hit || Contact.PocketId!=Pocket.Definition.Id || Contact.Time!=0 || !Unit(Contact.Normal) ||
       !Contact.Point.IsFinite() || Contact.InitiallyOverlapping ||
       !NearlyEqual(Length(Ball.PositionMeters-Contact.Point),Ball.RadiusMeters,1e-8))
        return Fail(Error,"invalid or penetrating pocket response contact");
    BallState probe=Ball;probe.RadiusMeters+=1e-8;
    if(!FindPocketContact(probe,Pocket,0,actual,Error) || !actual.Hit ||
       !actual.Point.NearlyEquals(Contact.Point,1e-7) || !actual.Normal.NearlyEquals(Contact.Normal,1e-6) ||
       actual.Feature!=Contact.Feature)return Fail(Error,"stale pocket response contact");
    const double speed=Ball.LinearVelocityMetersPerSecond.Dot(Contact.Normal);
    if(speed>=0)return true;
    const double restitution=Contact.Feature==PocketFeature::Bottom?0:Pocket.Parameters.JawRestitution;
    auto result=Ball;
    result.LinearVelocityMetersPerSecond+=Contact.Normal*(-(1+restitution)*speed);
    result.Sleeping=false;result.SleepTimeSeconds=0;result.Contact={EContactKind::Pocket,0,Contact.Normal};
    if(!result.IsValid(Error))return false;
    Ball=result;return true;
}
}
