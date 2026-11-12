#include <Snooker/Replay/ShotRecord.h>
#include <Snooker/Serialization/Text.h>
#include <iomanip>
#include <locale>
#include <set>
#include <sstream>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message) { if(Error) *Error=Message; return false; }
bool Token(const std::string& Value) { return !Value.empty() && Value.find_first_of(" \t\r\n")==std::string::npos; }
bool Tag(std::istream& s,const char* expected) { std::string name; return bool(s>>name) && name==expected; }
}
bool ShotRecord::IsValid(std::string* Error) const
{
    if(!Token(BuildVersion) || !Token(Result)) return Fail(Error,"invalid build or result token");
    if(!Parameters.IsValid(Error) || !Initial.IsValid(Error) || !Final.IsValid(Error) || !Input.IsValid(Error)) return false;
    if(Initial.Phase!=ESnapshotPhase::BeforeShot || Final.Phase!=ESnapshotPhase::AfterShot ||
       Initial.PhysicsParameterVersion!=Parameters.ParameterVersion || Final.PhysicsParameterVersion!=Parameters.ParameterVersion ||
       Initial.FixedStepSeconds!=Parameters.Solver.FixedStep || Final.FixedStepSeconds!=Parameters.Solver.FixedStep ||
       Final.StepIndex<Initial.StepIndex || Input.CueProfileId!=Parameters.Cue.Id ||
       Input.PullbackDistanceMeters>Parameters.Cue.MaxPullbackMeters) return Fail(Error,"record version, clock or cue mismatch");
    std::set<BallId> ids, finalIds, configuredIds;
    for(const auto& b:Initial.Balls) ids.insert(b.Id);
    for(const auto& b:Final.Balls) finalIds.insert(b.Id);
    for(const auto& b:Parameters.Balls) configuredIds.insert(b.Id);
    if(ids!=finalIds || ids!=configuredIds || !ids.count(Input.CueBallId)) return Fail(Error,"record ball set mismatch");
    for(const auto& b:Initial.Balls) if(b.Id==Input.CueBallId && !Input.ValidateFor(Parameters.Cue,b,Error)) return false;
    if(Events.size()>1000000) return Fail(Error,"event resource limit exceeded");
    std::uint64_t previousStep=Initial.StepIndex;
    Seconds previousTime=Initial.SimulationTimeSeconds;
    for(const auto& e:Events) {
        if(!Token(e.Kind) || !ids.count(e.Ball) || (e.OtherBall && (!ids.count(e.OtherBall) || e.OtherBall==e.Ball)) ||
           !e.Point.IsFinite() || !e.Normal.IsFinite() || !IsFinite(e.Time) || e.Time<previousTime ||
           e.Time>Final.SimulationTimeSeconds || e.Step<previousStep || e.Step>Final.StepIndex)
            return Fail(Error,"invalid or out-of-order shot event");
        previousStep=e.Step; previousTime=e.Time;
    }
    return true;
}
std::string ShotRecord::Serialize() const
{
    std::ostringstream s; s.imbue(std::locale::classic()); s<<std::setprecision(std::numeric_limits<double>::max_digits10);
    s<<"snooker-shot-v1 "<<BuildVersion
     <<"\nconfig "<<std::quoted(Parameters.Serialize())
     <<"\ninitial "<<std::quoted(Initial.Serialize())
     <<"\ninput "<<std::quoted(Input.Serialize())
     <<"\nevents "<<Events.size();
    for(const auto& e:Events) s<<"\nevent "<<e.Step<<' '<<e.Time<<' '<<e.Kind<<' '<<e.Ball<<' '<<e.OtherBall<<' '
        <<e.Point.X<<' '<<e.Point.Y<<' '<<e.Point.Z<<' '<<e.Normal.X<<' '<<e.Normal.Y<<' '<<e.Normal.Z;
    s<<"\nfinal "<<std::quoted(Final.Serialize())<<"\nresult "<<Result<<"\nend\n";
    return s.str();
}
bool ShotRecord::Deserialize(const std::string& Text, ShotRecord& Output, std::string* Error)
{
    if(Text.size()>64*1024*1024) return Fail(Error,"shot file exceeds 64 MiB limit");
    std::istringstream s(Text); s.imbue(std::locale::classic()); ShotRecord r;
    std::string blob; std::size_t count=0;
    if(!Tag(s,"snooker-shot-v1") || !(s>>r.BuildVersion) || !Tag(s,"config") || !(s>>std::quoted(blob)))
        return Fail(Error,"invalid shot header/config block");
    if(!CoreConfig::Deserialize(blob,r.Parameters,Error)) return false;
    if(!Tag(s,"initial") || !(s>>std::quoted(blob))) return Fail(Error,"missing initial snapshot");
    if(!PhysicsSnapshot::Deserialize(blob,r.Initial,Error)) return false;
    if(!Tag(s,"input") || !(s>>std::quoted(blob))) return Fail(Error,"missing shot input");
    if(!FShotInput::Deserialize(blob,r.Input,Error)) return false;
    if(!Tag(s,"events") || !ReadUnsigned(s,count) || count>1000000) return Fail(Error,"invalid event count");
    for(std::size_t i=0;i<count;++i) {
        ShotEventRecord e;
        if(!Tag(s,"event") || !ReadUnsigned(s,e.Step) || !(s>>e.Time>>e.Kind) || !ReadUnsigned(s,e.Ball) ||
           !ReadUnsigned(s,e.OtherBall) || !(s>>e.Point.X>>e.Point.Y>>e.Point.Z>>e.Normal.X>>e.Normal.Y>>e.Normal.Z))
            return Fail(Error,"invalid event record");
        r.Events.push_back(e);
    }
    if(!Tag(s,"final") || !(s>>std::quoted(blob))) return Fail(Error,"missing final snapshot");
    if(!PhysicsSnapshot::Deserialize(blob,r.Final,Error)) return false;
    if(!Tag(s,"result") || !(s>>r.Result) || !Tag(s,"end")) return Fail(Error,"invalid result or terminator");
    s>>std::ws;
    if(!s.eof()) return Fail(Error,"unexpected shot record tail");
    if(!r.IsValid(Error)) return false;
    Output=std::move(r); return true;
}
}
