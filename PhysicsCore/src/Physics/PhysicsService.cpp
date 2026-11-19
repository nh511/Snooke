#include <Snooker/Service/ISnookerPhysicsService.h>
#include <Snooker/Physics/BallContact.h>
#include <Snooker/Physics/ClothFriction.h>
#include <Snooker/Physics/CushionResponse.h>
#include <Snooker/Physics/Sleep.h>
#include <Snooker/Shot/CueImpulse.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <tuple>

namespace Snooker
{
std::string PhysicsEvent::Serialize() const
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(17)
      <<Sequence<<' '<<Step<<' '<<Time<<' '<<unsigned(Kind)<<' '<<Ball<<' '<<Other<<' '
      <<Point.ToString()<<' '<<Normal.ToString()<<' '<<Impulse;return s.str();
}
namespace
{
bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}
bool Active(const PocketStateData& p){return p.State!=PocketState::Potted;}
bool Cloth(const PocketStateData& p){return p.State!=PocketState::InPocket && Active(p);}
// Analytic planar sphere/finite-segment sweep (face plus both end caps).
bool SweepWall(const BallState& ball,const GeometrySegment& wall,Seconds horizon,Seconds& time,Vec3d& normal)
{
    Vec3d start=wall.Start,end=wall.End,pos=ball.PositionMeters,v=ball.LinearVelocityMetersPerSecond;
    start.Z=end.Z=pos.Z=v.Z=0;
    const auto delta=end-start;const auto length=delta.Length();if(length<=1e-12)return false;
    const auto tangent=delta*(1/length);const Vec3d n{-tangent.Y,tangent.X,0};
    bool hit=false;time=horizon;
    auto offer=[&](Seconds t,const Vec3d& candidate){
        if(t>=-1e-12 && t<=time && v.Dot(candidate)<-1e-7){time=std::max(0.0,t);normal=candidate;hit=true;}
    };
    const auto from=pos-start;const auto height=from.Dot(n),speed=v.Dot(n);
    if(speed!=0)for(double side:{-1.0,1.0}) {
        const double t=(side*ball.RadiusMeters-height)/speed;
        const double along=(from+v*t).Dot(tangent);
        if(along>=0 && along<=length)offer(t,n*side);
    }
    for(const auto& point:{start,end}) {
        const auto d=pos-point;const double aa=v.Dot(v),bb=d.Dot(v),cc=d.Dot(d)-ball.RadiusMeters*ball.RadiusMeters;
        if(aa==0 || bb>=0)continue;
        const double disc=bb*bb-aa*cc;if(disc<0)continue;
        const double denominator=-bb+std::sqrt(disc);if(denominator<=0)continue;
        const double t=cc/denominator;const auto at=d+v*t;const auto len=at.Length();
        if(len>0)offer(t,at*(1/len));
    }
    return hit;
}
struct Candidate
{
    int Type=0; // ball, wall, pocket contact, mouth, return
    std::size_t A=0,B=0;
    Seconds Time=0;
    Vec3d Normal;
    PocketContact Contact;
};
class Service final:public ISnookerPhysicsService
{
public:
    explicit Service(const PhysicsBundle& b):Bundle(b){GenerateTableGeometry(b.Table,b.Cushions,b.Pockets,b.Geometry,Geometry);}
    Service(const Service& Other):Bundle(Other.Bundle),Geometry(Other.Geometry),State(Other.State),
        Events(Other.Events),Fault(Other.Fault),Ready(Other.Ready) {}
    Service(Service&&)=default;
    Service& operator=(Service&&)=default;
    bool RestoreSnapshot(const PhysicsSnapshot& s,std::string* e) override
    {
        if(!s.IsValid(e))return false;
        if(s.Balls.empty() || s.Balls.size()>64 || s.PhysicsParameterVersion!=Bundle.Version || s.FixedStepSeconds!=Bundle.FixedStep)
            return Fail(e,"service snapshot/config mismatch or ball limit exceeded");
        auto next=s;if(next.PocketStates.empty())next.PocketStates.resize(next.Balls.size());
        for(std::size_t i=0;i<next.Balls.size();++i) {
            const auto& b=next.Balls[i];const auto& p=next.PocketStates[i];
            if(Cloth(p) && (std::abs(b.PositionMeters.Z-b.RadiusMeters)>1e-8 || b.LinearVelocityMetersPerSecond.Z!=0))
                return Fail(e,"service supports cloth-plane shots, not jumps");
            if(b.Sleeping && (b.LinearVelocityMetersPerSecond.LengthSquared()!=0 || b.AngularVelocityRadiansPerSecond.LengthSquared()!=0))
                return Fail(e,"sleeping ball has velocity");
            if(p.PocketId && std::none_of(Geometry.Pockets.begin(),Geometry.Pockets.end(),[&](const auto& x){return x.Definition.Id==p.PocketId;}))
                return Fail(e,"unknown pocket id");
        }
        // Canonicalize paired state, not just balls, for input-order independence.
        for(std::size_t i=0;i<next.Balls.size();++i)for(std::size_t j=i+1;j<next.Balls.size();++j)
            if(next.Balls[j].Id<next.Balls[i].Id){std::swap(next.Balls[i],next.Balls[j]);std::swap(next.PocketStates[i],next.PocketStates[j]);}
        State=std::move(next);Events.clear();PerformanceSamples.clear();Fault={};Ready=true;return true;
    }
    bool BeginShot(const PhysicsSnapshot& snapshot,const ShotInput& input,std::string* e) override
    {
        Service next(Bundle);if(!next.RestoreSnapshot(snapshot,e))return false;
        if(!next.IsSettled())return Fail(e,"shot requires settled initial state");
        if(!input.IsValid(e) || input.CueDirection.Z>0)return Fail(e,"upward cue direction unsupported");
        auto it=std::find_if(next.State.Balls.begin(),next.State.Balls.end(),[&](const auto& b){return b.Id==input.CueBallId;});
        if(it==next.State.Balls.end())return Fail(e,"cue ball not present");
        const auto i=static_cast<std::size_t>(it-next.State.Balls.begin());
        if(!Cloth(next.State.PocketStates[i]))return Fail(e,"cue ball is not on cloth");
        CueImpulseResult impulse;if(!ApplyCueImpulse(*it,input,Bundle.Cue,impulse,e))return false;
        it->LinearVelocityMetersPerSecond.Z=0;next.State.Phase=ESnapshotPhase::DuringStep;
        if(!next.Emit(PhysicsEventKind::CueStrike,it->Id,0,impulse.ContactPointMeters,input.CueDirection,impulse.ImpulseNewtonSeconds,next.State.SimulationTimeSeconds,e))return false;
        *this=std::move(next);return true;
    }
    bool Step(Seconds frame,PhysicsRunState& output,std::string* e) override
    {
        if(!Ready)return Fail(e,"service not initialized");
        if(!IsFinite(frame) || frame<0 || frame>3600-State.AccumulatorSeconds)return Fail(e,"invalid service frame time");
        if(Fault.Code!=PhysicsFaultCode::None){output=PhysicsRunState::Faulted;return true;}
        if(IsSettled() && State.Phase==ESnapshotPhase::AfterShot){output=PhysicsRunState::Settled;return true;}
        State.AccumulatorSeconds+=frame;
        for(unsigned budget=0;budget<1024 && State.AccumulatorSeconds+Bundle.FixedStep*1e-10>=Bundle.FixedStep;++budget) {
            auto next=*this;std::string message;PhysicsFault detected;PhysicsPerformanceSample performance;
            bool ok=CheckPhysicsSafety(State,Geometry,Bundle.Safety,0,detected,&message);
            if(ok)ok=next.Fixed(performance,&message);
            if(!ok) {
                Fault=detected.Code!=PhysicsFaultCode::None?detected:next.Fault;
                if(Fault.Code==PhysicsFaultCode::None){Fault.Code=message.find("budget")!=std::string::npos?PhysicsFaultCode::EventLoop:PhysicsFaultCode::SolverError;
                    Fault.Step=State.StepIndex+1;Fault.Time=Fault.Step*Bundle.FixedStep;Fault.Message=message;}
                Fault.Snapshot=State;Fault.ReplayConfig=Bundle.Serialize();if(e)*e=message;output=PhysicsRunState::Faulted;return true;
            }
            next.PerformanceSamples=std::move(PerformanceSamples);next.PerformanceSamples.push_back(performance);
            *this=std::move(next);
            if(IsSettled()){State.Phase=ESnapshotPhase::AfterShot;State.AccumulatorSeconds=0;output=PhysicsRunState::Settled;return true;}
        }
        output=PhysicsRunState::Running;return true;
    }
    bool IsSettled() const override
    {
        if(!Ready)return false;
        for(std::size_t i=0;i<State.Balls.size();++i) {
            const auto& p=State.PocketStates[i];
            if(Active(p) && (!Cloth(p) || !State.Balls[i].Sleeping))return false;
        }
        return true;
    }
    const std::vector<PhysicsEvent>& GetEvents() const override{return Events;}
    const std::vector<PhysicsPerformanceSample>& GetPerformanceSamples() const override{return PerformanceSamples;}
    PhysicsSnapshot SaveSnapshot() const override{return State;}
    const PhysicsFault& GetFault() const override{return Fault;}
private:
    bool Emit(PhysicsEventKind kind,BallId a,BallId b,const Vec3d& point,const Vec3d& normal,Scalar impulse,Seconds time,std::string* e)
    {
        if(State.EventCount==std::numeric_limits<std::uint64_t>::max())return Fail(e,"event budget overflow");
        Events.push_back({State.EventCount++,State.StepIndex,time,kind,a,b,point,normal,impulse});return true;
    }
    bool Fixed(PhysicsPerformanceSample& performance,std::string* e)
    {
        const auto performanceStart=std::chrono::steady_clock::now();
        performance={};
        if(State.StepIndex==std::numeric_limits<std::uint64_t>::max())return Fail(e,"step overflow");
        const auto before=State.Balls;const auto startEvents=State.EventCount;const auto startEventIndex=Events.size();
        const auto time0=State.SimulationTimeSeconds;
        ++State.StepIndex;const Seconds dt=Bundle.FixedStep/Bundle.Substeps;
        for(unsigned sub=0;sub<Bundle.Substeps;++sub) {
            for(std::size_t i=0;i<State.Balls.size();++i) {
                auto& b=State.Balls[i];const auto& p=State.PocketStates[i];
                if(Cloth(p)) {
                    if(!ApplyAngularUpdate(b,Bundle.Cloth,{Bundle.Gravity},dt,e) || !ApplyRollingResistance(b,Bundle.Cloth,{Bundle.Gravity},dt,e))return false;
                } else if(Active(p)){b.LinearVelocityMetersPerSecond.Z-=Bundle.Gravity*dt;b.Sleeping=false;}
            }
            Seconds remaining=dt,elapsed=0;
            for(unsigned iteration=0;remaining>0;++iteration) {
                if(iteration>Bundle.Safety.MaxEventsPerStep || State.EventCount-startEvents>Bundle.Safety.MaxEventsPerStep)return Fail(e,"collision event budget exceeded");
                const auto searchStart=std::chrono::steady_clock::now();
                Candidate best;best.Time=remaining;bool found=false;
                auto offer=[&](Candidate c){
                    if(c.Time<0 || c.Time>remaining)return;
                    if(!found || c.Time<best.Time-1e-12 || (std::abs(c.Time-best.Time)<=1e-12 &&
                       std::tie(c.Type,c.A,c.B)<std::tie(best.Type,best.A,best.B))){best=c;found=true;}
                };
                for(std::size_t i=0;i<State.Balls.size();++i) {
                    auto& b=State.Balls[i];const auto& state=State.PocketStates[i];if(!Active(state))continue;
                    for(std::size_t j=i+1;j<State.Balls.size();++j)if(Active(State.PocketStates[j])) {
                        if(b.LinearVelocityMetersPerSecond.NearlyEquals(State.Balls[j].LinearVelocityMetersPerSecond,0))continue;
                        BallContactTime hit;if(!FindBallContactTime(b,State.Balls[j],remaining,hit,e))return false;
                        if(hit.Hit && (State.Balls[j].LinearVelocityMetersPerSecond-b.LinearVelocityMetersPerSecond).Dot(hit.Normal)<-1e-7)
                            offer({0,i,j,hit.Time,hit.Normal,{}});
                    }
                    if(b.LinearVelocityMetersPerSecond.LengthSquared()==0)continue;
                    if(Cloth(state))for(std::size_t j=0;j<Geometry.Segments.size();++j) {
                        const auto& wall=Geometry.Segments[j];
                        if(wall.Feature==TableFeature::ClothBoundary || wall.Feature==TableFeature::CushionTop)continue;
                        Seconds t;Vec3d n;if(SweepWall(b,wall,remaining,t,n))offer({1,i,j,t,n,{}});
                    }
                    for(std::size_t j=0;j<Geometry.Pockets.size();++j) {
                        const auto& p=Geometry.Pockets[j];if(!Cloth(state) && state.PocketId!=p.Definition.Id)continue;
                        if(!Cloth(state)) {
                            PocketContact hit;if(!FindPocketContact(b,p,remaining,hit,e,true))return false;
                            if(hit.Hit)offer({2,i,j,hit.Time,hit.Normal,hit});
                        }
                        const auto from=b.PositionMeters-p.Definition.Center;
                        const auto x=from.Dot(p.Inward),speed=b.LinearVelocityMetersPerSecond.Dot(p.Inward);
                        if((Cloth(state) && speed<0 && x>=-1e-12) || (!Cloth(state) && speed>0 && x<=1e-12)) {
                            const Seconds t=std::max(0.0,-x/speed);const auto at=b.PositionMeters+b.LinearVelocityMetersPerSecond*t;
                            if(std::abs((at-p.Definition.Center).Dot(p.Tangent))<=p.Definition.Opening/2-b.RadiusMeters &&
                               (Cloth(state) || at.Z>=b.RadiusMeters-0.002))offer({Cloth(state)?3:4,i,j,t,p.Inward,{}});
                        }
                    }
                }
                performance.SearchNanoseconds+=static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now()-searchStart).count());
                const auto travel=found?best.Time:remaining;
                for(std::size_t i=0;i<State.Balls.size();++i)if(Active(State.PocketStates[i]))
                    State.Balls[i].PositionMeters+=State.Balls[i].LinearVelocityMetersPerSecond*travel;
                remaining=std::max(0.0,remaining-travel);elapsed+=travel;if(!found)break;
                auto& b=State.Balls[best.A];auto& pstate=State.PocketStates[best.A];const Seconds now=time0+sub*dt+elapsed;
                const auto resolutionStart=std::chrono::steady_clock::now();
                if(best.Type==0) {
                    BallCollisionResponse response;if(!ResolveBallContact(b,State.Balls[best.B],Bundle.BallCollision,response,e))return false;
                    if(Cloth(pstate))b.LinearVelocityMetersPerSecond.Z=0;
                    if(Cloth(State.PocketStates[best.B]))State.Balls[best.B].LinearVelocityMetersPerSecond.Z=0;
                    if(response.Applied && !Emit(PhysicsEventKind::BallBall,b.Id,State.Balls[best.B].Id,response.Point,response.Normal,response.NormalImpulse,now,e))return false;
                } else if(best.Type==1) {
                    const auto& wall=Geometry.Segments[best.B];
                    const bool jaw=wall.Feature==TableFeature::PocketJaw;
                    if(jaw) {
                        const auto it=std::find_if(Geometry.Pockets.begin(),Geometry.Pockets.end(),[&](const auto& p){return p.Definition.Id==wall.OwnerId;});
                        if(it==Geometry.Pockets.end())return Fail(e,"unresolved jaw");
                        const auto impulse=-(1+it->Parameters.JawRestitution)*b.LinearVelocityMetersPerSecond.Dot(best.Normal)*b.MassKg;
                        b.LinearVelocityMetersPerSecond+=best.Normal*(impulse/b.MassKg);b.Sleeping=false;b.SleepTimeSeconds=0;
                        if(!Emit(PhysicsEventKind::Jaw,b.Id,wall.OwnerId,b.PositionMeters-best.Normal*b.RadiusMeters,best.Normal,impulse,now,e))return false;
                    } else {
                        auto flat=Geometry;
                        for(auto& segment:flat.Segments)segment.Start.Z=segment.End.Z=b.RadiusMeters;
                        std::vector<CushionContact> contacts;if(!FindCushionContacts(b,flat,Bundle.BallCollision.ContactTolerance,contacts,e))return false;
                        const auto it=std::find_if(contacts.begin(),contacts.end(),[&](const auto& c){return c.SegmentIndex==best.B;});
                        if(it==contacts.end())return Fail(e,"swept wall contact missing");
                        CushionResponse response;if(!ResolveCushionContact(b,flat,*it,Bundle.BallCollision.ContactTolerance,response,e))return false;
                        if(response.Applied && !Emit(PhysicsEventKind::Cushion,b.Id,wall.OwnerId,it->Point,it->Normal,response.NormalImpulse,now,e))return false;
                        // Cloth support absorbs vertical cushion slip in this non-jump model.
                        b.LinearVelocityMetersPerSecond.Z=0;
                    }
                    b.PositionMeters+=best.Normal*1e-10;
                } else {
                    const auto& pocket=Geometry.Pockets[best.B];
                    if(best.Type==3) {
                        pstate={PocketState::InPocket,pocket.Definition.Id,false};b.Sleeping=false;
                        if(!Emit(PhysicsEventKind::MouthEntered,b.Id,pocket.Definition.Id,b.PositionMeters,pocket.Inward,0,now,e))return false;
                    } else if(best.Type==4) {
                        pstate={PocketState::Ejected,0,false};b.PositionMeters.Z=b.RadiusMeters;b.LinearVelocityMetersPerSecond.Z=0;
                        if(!Emit(PhysicsEventKind::RecoveredToCloth,b.Id,pocket.Definition.Id,b.PositionMeters,pocket.Inward,0,now,e))return false;
                    } else {
                        auto contact=best.Contact;contact.Time=0;if(!ReflectPocketContact(b,pocket,contact,e))return false;
                        if(!Emit(PhysicsEventKind::PocketContact,b.Id,pocket.Definition.Id,contact.Point,contact.Normal,0,now,e))return false;
                        if(contact.Feature==PocketFeature::Bottom) {
                            pstate={PocketState::Potted,pocket.Definition.Id,true};b.Sleeping=true;
                            b.LinearVelocityMetersPerSecond={};b.AngularVelocityRadiansPerSecond={};
                            if(!Emit(PhysicsEventKind::Potted,b.Id,pocket.Definition.Id,contact.Point,contact.Normal,0,now,e))return false;
                        } else b.PositionMeters+=contact.Normal*1e-10;
                    }
                }
                const auto resolutionNanoseconds=static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now()-resolutionStart).count());
                if(best.Type==0)performance.BallBallNanoseconds+=resolutionNanoseconds;
                else if(best.Type==1 && Geometry.Segments[best.B].Feature==TableFeature::PocketJaw)performance.JawNanoseconds+=resolutionNanoseconds;
                else if(best.Type==1)performance.CushionNanoseconds+=resolutionNanoseconds;
                else if(best.Type==2)performance.PocketContactNanoseconds+=resolutionNanoseconds;
                else if(best.Type==3)performance.MouthEnteredNanoseconds+=resolutionNanoseconds;
                else if(best.Type==4)performance.RecoveredToClothNanoseconds+=resolutionNanoseconds;
            }
        }
        State.SimulationTimeSeconds=State.StepIndex*Bundle.FixedStep;
        State.AccumulatorSeconds=std::max(0.0,State.AccumulatorSeconds-Bundle.FixedStep);
        for(std::size_t i=0;i<State.Balls.size();++i)if(Cloth(State.PocketStates[i])) {
            auto& b=State.Balls[i];const bool wasSleeping=before[i].Sleeping;
            if(!UpdateSleep(b,before[i].PositionMeters,Bundle.Cloth,Bundle.FixedStep,e))return false;
            if(!wasSleeping && b.Sleeping && !Emit(PhysicsEventKind::BallSettled,b.Id,0,b.PositionMeters,TableNormal,0,State.SimulationTimeSeconds,e))return false;
        }
        if(IsSettled() && !Emit(PhysicsEventKind::ShotSettled,0,0,{},{},0,State.SimulationTimeSeconds,e))return false;
        if(!CheckPhysicsSafety(State,Geometry,Bundle.Safety,State.EventCount-startEvents,Fault,e))return false;
        performance.Step=State.StepIndex;performance.Time=State.SimulationTimeSeconds;
        performance.ElapsedNanoseconds=static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now()-performanceStart).count());
        performance.TotalEvents=static_cast<std::uint32_t>(Events.size()-startEventIndex);
        for(std::size_t i=startEventIndex;i<Events.size();++i) {
            switch(Events[i].Kind) {
            case PhysicsEventKind::BallBall: ++performance.BallBall; break;
            case PhysicsEventKind::Cushion: ++performance.Cushion; break;
            case PhysicsEventKind::Jaw: ++performance.Jaw; break;
            case PhysicsEventKind::PocketContact: ++performance.PocketContact; break;
            case PhysicsEventKind::MouthEntered: ++performance.MouthEntered; break;
            case PhysicsEventKind::Potted: ++performance.Potted; break;
            case PhysicsEventKind::RecoveredToCloth: ++performance.RecoveredToCloth; break;
            case PhysicsEventKind::BallSettled: ++performance.BallSettled; break;
            case PhysicsEventKind::ShotSettled: ++performance.ShotSettled; break;
            case PhysicsEventKind::CueStrike: break;
            }
        }
        return true;
    }
    PhysicsBundle Bundle;TableGeometry Geometry;PhysicsSnapshot State;
    std::vector<PhysicsEvent> Events;std::vector<PhysicsPerformanceSample> PerformanceSamples;
    PhysicsFault Fault;bool Ready=false;
};
}
std::unique_ptr<ISnookerPhysicsService> CreatePhysicsService(const PhysicsBundle& bundle,std::string* e)
{
    if(!bundle.IsValid(e))return nullptr;
    if(!bundle.Geometry.RequireClosedBoundary){Fail(e,"full-table service requires a validated closed boundary");return nullptr;}
    return std::make_unique<Service>(bundle);
}
}
