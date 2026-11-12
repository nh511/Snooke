#include <Snooker/Shot/CueImpulse.h>
#include <algorithm>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) { if(Error) *Error=Message; return false; }
}
bool ApplyCueImpulse(BallState& Ball,const ShotInput& Input,const CueProfile& Cue,
                     CueImpulseResult& Result,std::string* Error)
{
    if(!Input.ValidateFor(Cue,Ball,Error)) return false;
    const auto& v=Ball.LinearVelocityMetersPerSecond;
    const auto& w=Ball.AngularVelocityRadiansPerSecond;
    if(v.X!=0 || v.Y!=0 || v.Z!=0 || w.X!=0 || w.Y!=0 || w.Z!=0)
        return Fail(Error,"cue impulse baseline requires a stationary ball");
    const Scalar length=std::hypot(Input.CueDirection.X,Input.CueDirection.Y,Input.CueDirection.Z);
    const Vec3d direction=Input.CueDirection*(1/length);
    // Remove only sub-tolerance parallel residue accepted by input geometry validation.
    const Vec3d offset=Input.CueTipOffsetMeters-direction*Input.CueTipOffsetMeters.Dot(direction);
    const Scalar radius=Ball.RadiusMeters;
    const Scalar normalizedOffset=std::hypot(offset.X,offset.Y,offset.Z)/radius;
    if(!IsFinite(normalizedOffset) || normalizedOffset>=1) return Fail(Error,"invalid sphere contact offset");
    const Vec3d lever=offset-direction*(radius*std::sqrt(std::max(0.0,1-normalizedOffset*normalizedOffset)));
    const Scalar inertia=0.4*Ball.MassKg*radius*radius;
    const Vec3d moment=lever.Cross(direction);
    if(!IsFinite(inertia) || inertia<=0) return Fail(Error,"invalid ball inertia");
    const Scalar inverseMass=1/Ball.MassKg+1/Cue.MassKg+moment.LengthSquared()/inertia;
    if(!IsFinite(inverseMass) || inverseMass<=0) return Fail(Error,"invalid effective mass");
    const Scalar rawImpulse=(1+Cue.TipRestitution)*Input.ForwardSpeedMetersPerSecond/inverseMass;
    if(!IsFinite(rawImpulse)) return Fail(Error,"cue impulse overflow");
    CueImpulseResult computed;
    computed.ImpulseNewtonSeconds=std::min(rawImpulse,Cue.MaxImpulseNewtonSeconds);
    computed.ContactPointMeters=Ball.PositionMeters+lever;
    computed.LinearDeltaMetersPerSecond=direction*(computed.ImpulseNewtonSeconds/Ball.MassKg);
    computed.AngularDeltaRadiansPerSecond=moment*(computed.ImpulseNewtonSeconds/inertia);
    if(!computed.ContactPointMeters.IsFinite() || !computed.LinearDeltaMetersPerSecond.IsFinite() ||
       !computed.AngularDeltaRadiansPerSecond.IsFinite()) return Fail(Error,"nonfinite impulse result");
    auto candidate=Ball;
    if(computed.ImpulseNewtonSeconds>0) {
        candidate.LinearVelocityMetersPerSecond+=computed.LinearDeltaMetersPerSecond;
        candidate.AngularVelocityRadiansPerSecond+=computed.AngularDeltaRadiansPerSecond;
        candidate.Sleeping=false; candidate.SleepTimeSeconds=0;
    }
    if(!candidate.IsValid(Error)) return false;
    Ball=candidate; Result=computed; return true;
}
}
