#include <Snooker/Replay/CollisionLog.h>
#include <Snooker/Serialization/Text.h>
#include <iomanip>
#include <locale>
#include <sstream>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) { if(Error)*Error=Message;return false; }
bool Tag(std::istream& s,const char* Expected) { std::string v;return bool(s>>v) && v==Expected; }
bool SameResponse(const BallCollisionResponse& a,const BallCollisionResponse& b)
{
    return a.Applied==b.Applied && a.NormalImpulse==b.NormalImpulse &&
        a.TangentialImpulse.NearlyEquals(b.TangentialImpulse,0) && a.Point.NearlyEquals(b.Point,0) &&
        a.Normal.NearlyEquals(b.Normal,0) && a.RelativeContactVelocity.NearlyEquals(b.RelativeContactVelocity,0) &&
        a.EnergyBefore==b.EnergyBefore && a.EnergyAfter==b.EnergyAfter;
}
bool ValidateEvent(const CollisionEvent& e,const BallCollisionProfile& p,std::string* Error)
{
    if(!IsFinite(e.Time) || e.Time<0) return Fail(Error,"invalid collision time");
    auto a=e.FirstBefore,b=e.SecondBefore;BallCollisionResponse expected;
    if(!ResolveBallContact(a,b,p,expected,Error))return false;
    if(!expected.Applied || a.Serialize()!=e.FirstAfter.Serialize() || b.Serialize()!=e.SecondAfter.Serialize() ||
       !SameResponse(expected,e.Response)) return Fail(Error,"collision event does not match response replay");
    return true;
}
void WriteVector(std::ostream& s,const Vec3d& v) { s<<v.X<<' '<<v.Y<<' '<<v.Z; }
bool ReadVector(std::istream& s,Vec3d& v) {return bool(s>>v.X>>v.Y>>v.Z);}
}
bool CollisionLog::IsValid(std::string* Error) const
{
    if(!Parameters.IsValid(Error))return false;
    if(Events.size()>100000) return Fail(Error,"collision event resource limit exceeded");
    Seconds time=0;StepIndex step=0;
    for(const auto& e:Events) {
        if(e.Time<time || e.Step<step) return Fail(Error,"collision events not chronological");
        if(!ValidateEvent(e,Parameters,Error)) return false;
        time=e.Time;step=e.Step;
    }
    return true;
}
bool CollisionLog::ResolveAndAppend(BallState& First,BallState& Second,StepIndex Step,Seconds Time,std::string* Error)
{
    if(&First==&Second || !IsFinite(Time) || Time<0 || Events.size()>=100000 ||
       (!Events.empty() && (Time<Events.back().Time || Step<Events.back().Step)))
        return Fail(Error,"invalid collision append order, aliases or resource count");
    CollisionEvent event;event.Step=Step;event.Time=Time;event.FirstBefore=First;event.SecondBefore=Second;
    auto a=First,b=Second;
    if(!ResolveBallContact(a,b,Parameters,event.Response,Error))return false;
    if(!event.Response.Applied)return true;
    event.FirstAfter=a;event.SecondAfter=b;
    Events.push_back(event);
    First=a;Second=b;return true;
}
std::string CollisionLog::Serialize() const
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<std::setprecision(std::numeric_limits<Scalar>::max_digits10);
    s<<"snooker-collisions-v1 "<<std::quoted(Parameters.Serialize())<<"\nevents "<<Events.size();
    for(const auto& e:Events) {
        const auto& r=e.Response;
        s<<"\nevent "<<e.Step<<' '<<e.Time<<' '<<std::quoted(e.FirstBefore.Serialize())<<' '<<std::quoted(e.SecondBefore.Serialize())
         <<' '<<std::quoted(e.FirstAfter.Serialize())<<' '<<std::quoted(e.SecondAfter.Serialize())<<' '<<r.NormalImpulse<<' ';
        WriteVector(s,r.TangentialImpulse);s<<' ';WriteVector(s,r.Point);s<<' ';WriteVector(s,r.Normal);s<<' ';
        WriteVector(s,r.RelativeContactVelocity);s<<' '<<r.EnergyBefore<<' '<<r.EnergyAfter;
    }
    s<<"\nend\n";return s.str();
}
bool CollisionLog::Deserialize(const std::string& Text,CollisionLog& Output,std::string* Error)
{
    if(Text.size()>64*1024*1024)return Fail(Error,"collision log exceeds 64 MiB");
    std::istringstream s(Text);s.imbue(std::locale::classic());CollisionLog log;std::string blob;std::size_t count=0;
    if(!Tag(s,"snooker-collisions-v1") || !(s>>std::quoted(blob)))return Fail(Error,"invalid collision header");
    if(!BallCollisionProfile::Deserialize(blob,log.Parameters,Error))return false;
    if(!Tag(s,"events") || !ReadUnsigned(s,count) || count>100000)return Fail(Error,"invalid collision event count");
    for(std::size_t i=0;i<count;++i) {
        CollisionEvent e;
        if(!Tag(s,"event") || !ReadUnsigned(s,e.Step) || !(s>>e.Time))return Fail(Error,"invalid collision event header");
        for(auto* b:{&e.FirstBefore,&e.SecondBefore,&e.FirstAfter,&e.SecondAfter}) {
            if(!(s>>std::quoted(blob)))return Fail(Error,"missing collision state");
            if(!BallState::Deserialize(blob,*b,Error))return false;
        }
        auto& r=e.Response;r.Applied=true;
        if(!(s>>r.NormalImpulse) || !ReadVector(s,r.TangentialImpulse) || !ReadVector(s,r.Point) ||
           !ReadVector(s,r.Normal) || !ReadVector(s,r.RelativeContactVelocity) || !(s>>r.EnergyBefore>>r.EnergyAfter))
            return Fail(Error,"invalid collision diagnostics");
        log.Events.push_back(e);
    }
    if(!Tag(s,"end"))return Fail(Error,"missing collision terminator");
    s>>std::ws;if(!s.eof())return Fail(Error,"unexpected collision log tail");
    if(!log.IsValid(Error))return false;
    Output=std::move(log);return true;
}
}
