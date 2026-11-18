#include <Snooker/Replay/PhysicsShotRecord.h>
#include <Snooker/Serialization/Text.h>
#include <iomanip>
#include <map>
#include <algorithm>

namespace Snooker
{
namespace
{
bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}
bool Tag(std::istream& s,const char* expected){std::string t;return bool(s>>t) && t==expected;}
bool Token(const std::string& t){return !t.empty() && t.find_first_of(" \t\r\n")==std::string::npos;}
}
bool PhysicsShotRecord::IsValid(std::string* Error) const
{
    if(!Token(BuildVersion) || !Parameters.IsValid(Error) || !Initial.IsValid(Error) || !Final.IsValid(Error) || !Input.IsValid(Error))return false;
    if(Initial.Phase!=ESnapshotPhase::BeforeShot || Initial.PhysicsParameterVersion!=Parameters.Version ||
       Final.PhysicsParameterVersion!=Parameters.Version || Initial.FixedStepSeconds!=Parameters.FixedStep ||
       Final.FixedStepSeconds!=Parameters.FixedStep || Final.StepIndex<Initial.StepIndex || Events.empty() || Events.size()>1000000)
        return Fail(Error,"record parameter/phase/clock/event mismatch");
    const bool settled=Result=="Settled";
    if(settled) {
        if(Final.Phase!=ESnapshotPhase::AfterShot || Fault.Code!=PhysicsFaultCode::None ||
           Events.back().Kind!=PhysicsEventKind::ShotSettled || Final.AccumulatorSeconds!=0)
            return Fail(Error,"record is not a completed settled shot");
    } else if(Result!=std::string("fault:")+PhysicsFaultCodeName(Fault.Code) || !Fault.IsValid(Error))
        return Fail(Error,"record fault result mismatch");
    std::map<BallId,const BallState*> initial,final;
    for(const auto& b:Initial.Balls)initial[b.Id]=&b;
    for(std::size_t i=0;i<Final.Balls.size();++i) {
        const auto& b=Final.Balls[i];final[b.Id]=&b;
        const auto p=Final.PocketStates.empty()?PocketStateData{}:Final.PocketStates[i];
        if(settled && p.State!=PocketState::Potted && (!b.Sleeping || p.State==PocketState::InPocket))
            return Fail(Error,"record contains unsettled final ball");
        const auto found=initial.find(b.Id);
        if(found==initial.end() || b.RadiusMeters!=found->second->RadiusMeters || b.MassKg!=found->second->MassKg ||
           b.MaterialProfileId!=found->second->MaterialProfileId)return Fail(Error,"record ball definitions changed");
    }
    if(initial.size()!=final.size() || !initial.count(Input.CueBallId) ||
       !Input.ValidateFor(Parameters.Cue,*initial[Input.CueBallId],Error))return Fail(Error,"record initial input/ball mismatch");
    for(const auto& b:Initial.Balls)if(!b.Sleeping || b.LinearVelocityMetersPerSecond.LengthSquared()!=0 || b.AngularVelocityRadiansPerSecond.LengthSquared()!=0)
        return Fail(Error,"record must start from stationary balls");
    if(Events.front().Kind!=PhysicsEventKind::CueStrike || Events.front().Ball!=Input.CueBallId)
        return Fail(Error,"record lacks initial cue strike");
    auto sequence=Initial.EventCount;auto step=Initial.StepIndex;auto time=Initial.SimulationTimeSeconds;
    for(const auto& e:Events) {
        if(unsigned(e.Kind)>unsigned(PhysicsEventKind::ShotSettled) || e.Sequence!=sequence++ || e.Step<step ||
           e.Step>Final.StepIndex || !IsFinite(e.Time) || e.Time<time || e.Time>Final.SimulationTimeSeconds+1e-12 ||
           !e.Point.IsFinite() || !e.Normal.IsFinite() || !IsFinite(e.Impulse) || e.Impulse<0)
            return Fail(Error,"invalid or unordered physics event");
        if(e.Kind==PhysicsEventKind::ShotSettled) {
            if(e.Ball || e.Other)return Fail(Error,"settled event has ball IDs");
        } else if(!initial.count(e.Ball))return Fail(Error,"event references unknown ball");
        if(e.Kind==PhysicsEventKind::BallBall && (!initial.count(e.Other) || e.Ball==e.Other))
            return Fail(Error,"invalid collision IDs");
        if(e.Kind==PhysicsEventKind::Cushion && std::none_of(Parameters.Table.Cushions.begin(),Parameters.Table.Cushions.end(),[&](const auto& c){return c.Id==e.Other;}))
            return Fail(Error,"event references unknown rail");
        if((e.Kind==PhysicsEventKind::Jaw || e.Kind==PhysicsEventKind::PocketContact || e.Kind==PhysicsEventKind::MouthEntered ||
            e.Kind==PhysicsEventKind::Potted || e.Kind==PhysicsEventKind::RecoveredToCloth) &&
            std::none_of(Parameters.Table.Pockets.begin(),Parameters.Table.Pockets.end(),[&](const auto& p){return p.Id==e.Other;}))
            return Fail(Error,"event references unknown pocket");
        step=e.Step;time=e.Time;
    }
    if(sequence!=Final.EventCount)return Fail(Error,"final event cursor mismatch");
    return true;
}
std::string PhysicsShotRecord::Serialize() const
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(17)
      <<"snooker-shot-v2 "<<BuildVersion<<"\nbundle "<<std::quoted(Parameters.Serialize())
      <<"\ninitial "<<std::quoted(Initial.Serialize())<<"\ninput "<<std::quoted(Input.Serialize())
      <<"\nevents "<<Events.size();
    for(const auto& e:Events)s<<"\nevent "<<e.Sequence<<' '<<e.Step<<' '<<e.Time<<' '<<unsigned(e.Kind)<<' '<<e.Ball<<' '<<e.Other<<' '
      <<e.Point.X<<' '<<e.Point.Y<<' '<<e.Point.Z<<' '<<e.Normal.X<<' '<<e.Normal.Y<<' '<<e.Normal.Z<<' '<<e.Impulse;
    s<<"\nfinal "<<std::quoted(Final.Serialize())<<"\nresult "<<Result;
    if(Fault.Code!=PhysicsFaultCode::None)s<<"\nfault "<<std::quoted(Fault.Serialize());
    s<<"\nend\n";return s.str();
}
bool PhysicsShotRecord::Deserialize(const std::string& text,PhysicsShotRecord& output,std::string* error)
{
    if(text.size()>64*1024*1024)return Fail(error,"shot file exceeds 64 MiB");
    PhysicsShotRecord r;std::istringstream s(text);s.imbue(std::locale::classic());std::string blob;
    if(!Tag(s,"snooker-shot-v2") || !(s>>r.BuildVersion) || !Tag(s,"bundle") || !(s>>std::quoted(blob)) ||
       !PhysicsBundle::Deserialize(blob,r.Parameters,error))return Fail(error,"invalid shot bundle/header");
    if(!Tag(s,"initial") || !(s>>std::quoted(blob)) || !PhysicsSnapshot::Deserialize(blob,r.Initial,error) ||
       !Tag(s,"input") || !(s>>std::quoted(blob)) || !ShotInput::Deserialize(blob,r.Input,error))return Fail(error,"invalid shot input/snapshot");
    std::size_t count=0;if(!Tag(s,"events") || !ReadUnsigned(s,count) || count>1000000)return Fail(error,"invalid shot event count");
    for(std::size_t i=0;i<count;++i) {
        PhysicsEvent e;unsigned kind=0;
        if(!Tag(s,"event") || !ReadUnsigned(s,e.Sequence) || !ReadUnsigned(s,e.Step) || !(s>>e.Time) || !ReadUnsigned(s,kind) ||
           kind>unsigned(PhysicsEventKind::ShotSettled) || !ReadUnsigned(s,e.Ball) || !ReadUnsigned(s,e.Other) ||
           !(s>>e.Point.X>>e.Point.Y>>e.Point.Z>>e.Normal.X>>e.Normal.Y>>e.Normal.Z>>e.Impulse))return Fail(error,"invalid physics event row");
        e.Kind=static_cast<PhysicsEventKind>(kind);r.Events.push_back(e);
    }
    if(!Tag(s,"final") || !(s>>std::quoted(blob)) || !PhysicsSnapshot::Deserialize(blob,r.Final,error) ||
       !Tag(s,"result") || !(s>>r.Result))return Fail(error,"invalid shot final/result");
    if(r.Result!="Settled" && (!Tag(s,"fault") || !(s>>std::quoted(blob)) || !PhysicsFault::Deserialize(blob,r.Fault,error)))
        return Fail(error,"missing shot fault");
    if(!Tag(s,"end") || !(s>>std::ws).eof())return Fail(error,"invalid shot tail");
    if(!r.IsValid(error))return false;output=std::move(r);return true;
}
}
