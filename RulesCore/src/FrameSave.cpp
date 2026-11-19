#include <Snooker/Rules/FrameSave.h>
#include <Snooker/Serialization/Text.h>
#include <iomanip>
#include <set>

namespace Snooker
{
namespace {bool Fail(std::string* e,const char* m){if(e)*e=m;return false;}bool Tag(std::istream& s,const char* x){std::string v;return bool(s>>v)&&v==x;}bool Token(const std::string& v){return !v.empty()&&v.find_first_of(" \t\r\n")==std::string::npos;}}
bool FrameSave::IsValid(std::string* Error) const
{
    if(!Token(GameVersion)||!Parameters.IsValid(Error)||!Match.IsValid(Error)||!Physics.IsValid(Error)||Physics.PhysicsParameterVersion!=Parameters.Version||Physics.FixedStepSeconds!=Parameters.FixedStep||Definitions.size()!=Physics.Balls.size()||Definitions.size()>64||Shots.size()>10000)return Fail(Error,"invalid frame save header/state/count");
    std::set<BallId> ids;unsigned cue=0;
    for(const auto& d:Definitions){if(!d.IsValid(Error)||!ids.insert(d.Id).second)return Fail(Error,"invalid frame definition");if(d.Type==EBallType::Cue)++cue;auto it=std::find_if(Physics.Balls.begin(),Physics.Balls.end(),[&](const auto& b){return b.Id==d.Id;});if(it==Physics.Balls.end()||it->RadiusMeters!=d.RadiusMeters||it->MassKg!=d.MassKg||it->MaterialProfileId!=d.MaterialProfileId)return Fail(Error,"frame definition/physics mismatch");}
    if(cue!=1)return Fail(Error,"frame save requires one cue ball");
    for(const auto& shot:Shots){if(!shot.IsValid(Error)||shot.Parameters.Serialize()!=Parameters.Serialize())return Fail(Error,"frame shot history parameter mismatch");std::set<BallId> shotIds;for(const auto& b:shot.Initial.Balls)shotIds.insert(b.Id);if(shotIds!=ids)return Fail(Error,"frame shot history ball set mismatch");}
    return true;
}
std::string FrameSave::Serialize() const
{
    std::ostringstream s;s.imbue(std::locale::classic());s<<"snooker-frame-v1 "<<GameVersion<<"\nbundle "<<std::quoted(Parameters.Serialize())<<"\nmatch "<<std::quoted(Match.Serialize())<<"\nphysics "<<std::quoted(Physics.Serialize())<<"\ndefinitions "<<Definitions.size();for(const auto& d:Definitions)s<<"\ndefinition "<<std::quoted(d.Serialize());s<<"\nshots "<<Shots.size();for(const auto& shot:Shots)s<<"\nshot "<<std::quoted(shot.Serialize());s<<"\nend\n";return s.str();
}
bool FrameSave::Deserialize(const std::string& text,FrameSave& output,std::string* Error)
{
    if(text.size()>128*1024*1024)return Fail(Error,"frame save exceeds 128 MiB");FrameSave f;std::istringstream s(text);s.imbue(std::locale::classic());std::string blob;std::size_t count=0;
    if(!Tag(s,"snooker-frame-v1")||!(s>>f.GameVersion)||!Tag(s,"bundle")||!(s>>std::quoted(blob))||!PhysicsBundle::Deserialize(blob,f.Parameters,Error)||!Tag(s,"match")||!(s>>std::quoted(blob))||!MatchState::Deserialize(blob,f.Match,Error)||!Tag(s,"physics")||!(s>>std::quoted(blob))||!PhysicsSnapshot::Deserialize(blob,f.Physics,Error)||!Tag(s,"definitions")||!ReadUnsigned(s,count)||count>64)return Fail(Error,"invalid frame save header");
    for(std::size_t i=0;i<count;++i){BallDefinition d;if(!Tag(s,"definition")||!(s>>std::quoted(blob))||!BallDefinition::Deserialize(blob,d,Error))return Fail(Error,"invalid frame definition row");f.Definitions.push_back(d);}
    if(!Tag(s,"shots")||!ReadUnsigned(s,count)||count>10000)return Fail(Error,"invalid frame shot count");for(std::size_t i=0;i<count;++i){PhysicsShotRecord shot;if(!Tag(s,"shot")||!(s>>std::quoted(blob))||!PhysicsShotRecord::Deserialize(blob,shot,Error))return Fail(Error,"invalid frame shot row");f.Shots.push_back(std::move(shot));}
    if(!Tag(s,"end")||!(s>>std::ws).eof()||!f.IsValid(Error))return Fail(Error,"invalid frame save tail/state");output=std::move(f);return true;
}
bool ContinueFrame(const FrameSave& frame,const ShotInput& input,FrameSave& output,PhysicsShotRecord* shotOut,std::string* Error)
{
    if(!frame.IsValid(Error)||frame.Match.Phase==FramePhase::Finished)return Fail(Error,"cannot continue invalid/finished frame");
    auto start=frame.Physics;start.Phase=ESnapshotPhase::BeforeShot;
    auto service=CreatePhysicsService(frame.Parameters,Error);if(!service||!service->BeginShot(start,input,Error))return false;
    PhysicsRunState run=PhysicsRunState::Running;for(unsigned n=0;n<1000000&&run==PhysicsRunState::Running;++n)if(!service->Step(1.0/60,run,Error))return false;
    if(run!=PhysicsRunState::Settled)return Fail(Error,"continued frame shot did not settle");
    PhysicsShotRecord shot;shot.Parameters=frame.Parameters;shot.Initial=start;shot.Input=input;shot.Final=service->SaveSnapshot();shot.Events=service->GetEvents();if(!shot.IsValid(Error))return false;
    auto next=frame;next.Physics=shot.Final;next.Shots.push_back(shot);if(!next.IsValid(Error))return false;output=std::move(next);if(shotOut)*shotOut=std::move(shot);return true;
}
}
