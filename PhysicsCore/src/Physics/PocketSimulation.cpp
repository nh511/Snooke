#include <Snooker/Service/PocketSimulation.h>
#include <Snooker/Physics/ClothFriction.h>
#include <Snooker/Physics/Sleep.h>
#include <Snooker/Serialization/Text.h>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <set>

namespace Snooker
{
namespace
{
bool Fail(std::string* e,const char* message) {if(e)*e=message;return false;}
bool Token(const std::string& t) {return !t.empty() && t.find_first_of(" \t\r\n")==std::string::npos;}
std::ostringstream Writer()
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(17);return s;
}
bool Tag(std::istream& s,const char* expected) {std::string t;return bool(s>>t) && t==expected;}
const PocketGeometry* Find(const TableGeometry& g,std::uint32_t id)
{
    for(const auto& p:g.Pockets)if(p.Definition.Id==id)return &p;
    return nullptr;
}
}
bool PocketSimulationConfig::IsValid(std::string* Error) const
{
    if(!Token(Version) || !IsFinite(FixedStep) || FixedStep<1e-6 || FixedStep>0.1 ||
       !IsFinite(Gravity) || Gravity<=0 || Gravity>100 || Substeps<1 || Substeps>128 ||
       !Cloth.IsValid(Error) || !BallCollision.IsValid(Error) || !Cue.IsValid(Error))return Fail(Error,"invalid pocket simulation parameters");
    if(!Safety.IsValid(Error))return false;
    TableGeometry g;
    return GenerateTableGeometry(Table,Cushions,Pockets,Geometry,g,Error);
}
std::string PocketSimulationConfig::Serialize() const
{
    auto s=Writer();s<<"snooker-physics-bundle-v1 "<<Version<<' '<<FixedStep<<' '<<Gravity<<' '<<Substeps
       <<' '<<Geometry.CoordinateTolerance<<' '<<Geometry.JawSubdivisions
       <<"\ncloth "<<Cloth.ParameterVersion<<' '<<Cloth.Condition<<' '<<Cloth.Sliding<<' '<<Cloth.Rolling<<' '
       <<Cloth.Spin<<' '<<Cloth.SleepLinearSpeed<<' '<<Cloth.SleepAngularSpeed<<' '<<Cloth.SleepPositionTolerance<<' '<<Cloth.SleepDuration
       <<"\ntable "<<std::quoted(Table.Serialize())<<"\ncushions "<<Cushions.size();
    for(const auto& p:Cushions)s<<' '<<std::quoted(p.Serialize());
    s<<"\npockets "<<Pockets.size();for(const auto& p:Pockets)s<<' '<<std::quoted(p.Serialize());
    s<<"\nball-contact "<<std::quoted(BallCollision.Serialize())
     <<"\ncue "<<std::quoted(Cue.Serialize());
    s<<"\nsafety "<<std::quoted(Safety.Serialize())<<"\nend";return s.str();
}
bool PocketSimulationConfig::Deserialize(const std::string& Text,PocketSimulationConfig& Output,std::string* Error)
{
    if(Text.size()>16*1024*1024)return Fail(Error,"pocket config exceeds 16 MiB");
    std::istringstream s(Text);s.imbue(std::locale::classic());PocketSimulationConfig c;std::string blob;
    if(!Tag(s,"snooker-physics-bundle-v1") || !(s>>c.Version>>c.FixedStep>>c.Gravity) ||
       !ReadUnsigned(s,c.Substeps) || !(s>>c.Geometry.CoordinateTolerance) || !ReadUnsigned(s,c.Geometry.JawSubdivisions) ||
       !Tag(s,"cloth") || !(s>>c.Cloth.ParameterVersion>>c.Cloth.Condition>>c.Cloth.Sliding>>c.Cloth.Rolling>>c.Cloth.Spin
          >>c.Cloth.SleepLinearSpeed>>c.Cloth.SleepAngularSpeed>>c.Cloth.SleepPositionTolerance>>c.Cloth.SleepDuration) ||
       !Tag(s,"table") || !(s>>std::quoted(blob)) || !TableDefinition::Deserialize(blob,c.Table,Error))
        return Fail(Error,"invalid pocket config header/table");
    std::size_t n=0;
    if(!Tag(s,"cushions") || !ReadUnsigned(s,n) || n>4096)return Fail(Error,"invalid cushion count");
    for(std::size_t i=0;i<n;++i) {
        CushionProfile p;if(!(s>>std::quoted(blob)) || !CushionProfile::Deserialize(blob,p,Error))return false;
        c.Cushions.push_back(p);
    }
    if(!Tag(s,"pockets") || !ReadUnsigned(s,n) || n>4096)return Fail(Error,"invalid pocket count");
    for(std::size_t i=0;i<n;++i) {
        PocketProfile p;if(!(s>>std::quoted(blob)) || !PocketProfile::Deserialize(blob,p,Error))return false;
        c.Pockets.push_back(p);
    }
    if(!Tag(s,"ball-contact") || !(s>>std::quoted(blob)) || !BallCollisionProfile::Deserialize(blob,c.BallCollision,Error) ||
       !Tag(s,"cue") || !(s>>std::quoted(blob)) || !CueProfile::Deserialize(blob,c.Cue,Error) ||
       !Tag(s,"safety") || !(s>>std::quoted(blob)) || !PhysicsSafetyLimits::Deserialize(blob,c.Safety,Error) ||
       !Tag(s,"end") || !(s>>std::ws).eof() || !c.IsValid(Error))return Fail(Error,"invalid pocket config tail/values");
    Output=std::move(c);return true;
}
std::string PocketSimulationEvent::Serialize() const
{
    auto s=Writer();s<<Sequence<<' '<<Step<<' '<<Time<<' '<<Ball<<' '<<Pocket<<' '<<unsigned(Kind)<<' '
       <<unsigned(Feature)<<' '<<Point.ToString()<<' '<<Normal.ToString()<<' '<<NormalSpeedBefore<<' '<<NormalSpeedAfter;return s.str();
}
bool PocketSimulation::Restore(const PocketSimulationConfig& c,const PhysicsSnapshot& snapshot,std::string* Error)
{
    if(!c.IsValid(Error) || !snapshot.IsValid(Error))return false;
    if(snapshot.Balls.size()!=1 || snapshot.PhysicsParameterVersion!=c.Version || snapshot.FixedStepSeconds!=c.FixedStep)
        return Fail(Error,"single-ball pocket service config/snapshot mismatch");
    PocketSimulation next;next.Config=c;next.State=snapshot;
    if(!GenerateTableGeometry(c.Table,c.Cushions,c.Pockets,c.Geometry,next.Geometry,Error))return false;
    if(next.State.PocketStates.empty())next.State.PocketStates.resize(1);
    const auto& p=next.State.PocketStates[0];const auto& ball=snapshot.Balls[0];
    if(p.PocketId && !Find(next.Geometry,p.PocketId))return Fail(Error,"unknown snapshot pocket id");
    if((p.State==PocketState::OnCloth || p.State==PocketState::Ejected) &&
       (std::abs(ball.PositionMeters.Z-ball.RadiusMeters)>1e-8 || std::abs(ball.LinearVelocityMetersPerSecond.Z)>1e-8))
        return Fail(Error,"cloth ball must rest on the cloth plane");
    if(ball.Sleeping && p.State!=PocketState::Potted &&
       (ball.LinearVelocityMetersPerSecond.LengthSquared()!=0 || ball.AngularVelocityRadiansPerSecond.LengthSquared()!=0))
        return Fail(Error,"sleeping ball has nonzero velocity");
    next.Ready=true;*this=std::move(next);return true;
}
bool PocketSimulation::Step(std::string* Error)
{
    if(!Ready)return Fail(Error,"pocket service not initialized");
    if(IsFaulted())return Fail(Error,"pocket service is faulted");
    PhysicsFault preflight;
    if(!CheckPhysicsSafety(State,Geometry,Config.Safety,0,preflight,Error)) {
        Fault=preflight;Fault.Snapshot=State;Fault.ReplayConfig=Config.Serialize();return false;
    }
    auto next=*this;std::string message;
    if(!next.StepInternal(&message)) {
        if(!next.IsFaulted()) {
            next.Fault.Code=message.find("contact limit")!=std::string::npos || message.find("cursor overflow")!=std::string::npos?
                PhysicsFaultCode::EventLoop:message.find("penetration")!=std::string::npos?
                PhysicsFaultCode::Penetration:PhysicsFaultCode::SolverError;
            next.Fault.Step=State.StepIndex+1;next.Fault.Time=(State.StepIndex+1)*State.FixedStepSeconds;
            next.Fault.Ball=State.Balls[0].Id;next.Fault.Message=message;
        }
        Fault=next.Fault;Fault.Snapshot=State;Fault.ReplayConfig=Config.Serialize();
        if(Error)*Error=message;
        return false;
    }
    *this=std::move(next);return true;
}
bool PocketSimulation::StepInternal(std::string* Error)
{
    if(State.StepIndex==std::numeric_limits<std::uint64_t>::max())return Fail(Error,"step counter overflow");
    const auto eventStart=State.EventCount;
    auto& ball=State.Balls[0];auto& pocketState=State.PocketStates[0];
    const auto previous=ball.PositionMeters;
    auto emit=[&](PocketSimulationEventKind kind,const PocketGeometry& p,Seconds t,const PocketContact* contact) {
        if(State.EventCount==std::numeric_limits<std::uint64_t>::max() ||
           State.EventCount-eventStart>=Config.Safety.MaxEventsPerStep)return false;
        Events.push_back({State.EventCount++,State.StepIndex+1,t,ball.Id,p.Definition.Id,kind,
           contact?contact->Feature:PocketFeature::Jaw,contact?contact->Point:ball.PositionMeters,
           contact?contact->Normal:p.Inward});return true;
    };
    const Seconds dt=Config.FixedStep/Config.Substeps;
    for(std::uint32_t sub=0;sub<Config.Substeps;++sub) {
        if(pocketState.State==PocketState::Potted)break;
        const bool cloth=pocketState.State!=PocketState::InPocket;
        if(cloth) {
            if(!ApplyAngularUpdate(ball,Config.Cloth,{Config.Gravity},dt,Error) ||
               !ApplyRollingResistance(ball,Config.Cloth,{Config.Gravity},dt,Error))return false;
        } else {
            ball.LinearVelocityMetersPerSecond.Z-=Config.Gravity*dt;ball.Sleeping=false;
        }
        Seconds remaining=dt,elapsed=0;
        for(unsigned iterations=0;remaining>0;++iterations) {
            if(iterations>=Config.Safety.MaxEventsPerStep)return Fail(Error,"pocket substep contact limit exceeded");
            const PocketGeometry* chosen=nullptr;PocketContact contact;
            Seconds earliest=remaining;int action=0; // 1 contact, 2 mouth entry, 3 recovery
            for(const auto& p:Geometry.Pockets) {
                if(pocketState.State==PocketState::InPocket && pocketState.PocketId!=p.Definition.Id)continue;
                PocketContact hit;
                if(!FindPocketContact(ball,p,remaining,hit,Error,true))return false;
                if(hit.Hit && hit.Time<=earliest) {earliest=hit.Time;contact=hit;chosen=&p;action=1;}
                const auto relative=ball.PositionMeters-p.Definition.Center;
                const Scalar x=relative.Dot(p.Inward),vx=ball.LinearVelocityMetersPerSecond.Dot(p.Inward);
                if((pocketState.State!=PocketState::InPocket && vx<0 && x>=-1e-12) ||
                   (pocketState.State==PocketState::InPocket && vx>0 && x<=1e-12)) {
                    const Seconds t=std::max(0.0,-x/vx);
                    const auto at=ball.PositionMeters+ball.LinearVelocityMetersPerSecond*t;
                    if(t<=earliest && std::abs((at-p.Definition.Center).Dot(p.Tangent))<=p.Definition.Opening/2-ball.RadiusMeters &&
                       (pocketState.State!=PocketState::InPocket || at.Z>=ball.RadiusMeters-0.002)) {
                        earliest=t;chosen=&p;action=pocketState.State==PocketState::InPocket?3:2;
                    }
                }
            }
            ball.PositionMeters+=ball.LinearVelocityMetersPerSecond*earliest;
            elapsed+=earliest;remaining=std::max(0.0,remaining-earliest);
            const Seconds time=State.SimulationTimeSeconds+sub*dt+elapsed;
            if(!action)break;
            if(action==2) {
                pocketState={PocketState::InPocket,chosen->Definition.Id,false};ball.Sleeping=false;
                if(!emit(PocketSimulationEventKind::MouthEntered,*chosen,time,nullptr))return Fail(Error,"event cursor overflow");
            } else if(action==3) {
                PocketStateUpdate update;
                if(!UpdatePocketState(pocketState,nullptr,false,true,update,Error))return false;
                pocketState=update.State;ball.PositionMeters.Z=ball.RadiusMeters;ball.LinearVelocityMetersPerSecond.Z=0;
                if(!emit(PocketSimulationEventKind::RecoveredToCloth,*chosen,time,nullptr))return Fail(Error,"event cursor overflow");
            } else {
                contact.Time=0;
                if(contact.InitiallyOverlapping)return Fail(Error,"initial pocket penetration");
                const auto incoming=ball.LinearVelocityMetersPerSecond.Dot(contact.Normal);
                if(!ReflectPocketContact(ball,*chosen,contact,Error))return false;
                if(!emit(PocketSimulationEventKind::Contact,*chosen,time,&contact))return Fail(Error,"event cursor overflow");
                Events.back().NormalSpeedBefore=incoming;
                Events.back().NormalSpeedAfter=ball.LinearVelocityMetersPerSecond.Dot(contact.Normal);
                if(contact.Feature==PocketFeature::Bottom) {
                    if(ball.PositionMeters.Z> -chosen->Parameters.CaptureDepth+ball.RadiusMeters+1e-8)
                        return Fail(Error,"bottom above capture depth");
                    PocketStateUpdate update;
                    if(!UpdatePocketState(pocketState,&contact,true,false,update,Error))return false;
                    pocketState=update.State;ball.LinearVelocityMetersPerSecond={};ball.AngularVelocityRadiansPerSecond={};ball.Sleeping=true;
                    if(update.Event==PocketEvent::EnteredPocket && !emit(PocketSimulationEventKind::Potted,*chosen,time,&contact))
                        return Fail(Error,"event cursor overflow");
                    break;
                }
                // Remove boundary roundoff, not a penetration correction.
                ball.PositionMeters+=contact.Normal*1e-10;
            }
        }
    }
    if(pocketState.State!=PocketState::InPocket && pocketState.State!=PocketState::Potted &&
       !UpdateSleep(ball,previous,Config.Cloth,Config.FixedStep,Error))return false;
    ++State.StepIndex;State.SimulationTimeSeconds=State.StepIndex*State.FixedStepSeconds;
    State.Phase=ESnapshotPhase::DuringStep;
    PhysicsFault detected;
    if(!CheckPhysicsSafety(State,Geometry,Config.Safety,State.EventCount-eventStart,detected,Error)) {Fault=detected;return false;}
    return true;
}
bool PocketSimulation::SaveFault(const std::string& Path,std::string* Error) const
{
    if(!IsFaulted())return Fail(Error,"no physics fault is available");
    if(!Fault.IsValid(Error))return false;
    std::ofstream File(Path);if(!File || !(File<<Fault.Serialize()))return Fail(Error,"cannot write physics fault");
    File.flush();if(!File)return Fail(Error,"physics fault flush failed");return true;
}
}
