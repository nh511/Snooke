#include <Snooker/Physics/PhysicsFault.h>
#include <Snooker/Serialization/Text.h>
#include <iomanip>
#include <locale>
#include <sstream>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message){if(Error)*Error=Message;return false;}
void SetFault(PhysicsFault& Output,PhysicsFaultCode Code,const PhysicsSnapshot& Snapshot,
              BallId Ball,const std::string& Message)
{
    Output=PhysicsFault{};Output.Code=Code;Output.Step=Snapshot.StepIndex;Output.Time=Snapshot.SimulationTimeSeconds;
    Output.Ball=Ball;Output.Message=Message;
    if(Snapshot.IsValid())Output.Snapshot=Snapshot;
    else {
        std::ostringstream diagnostic;diagnostic.imbue(std::locale::classic());
        diagnostic<<std::setprecision(17)<<"step="<<Snapshot.StepIndex<<" time="<<Snapshot.SimulationTimeSeconds
            <<" dt="<<Snapshot.FixedStepSeconds<<" debt="<<Snapshot.AccumulatorSeconds;
        for(const auto& ball:Snapshot.Balls)diagnostic<<"\nball "<<ball.Serialize();
        for(const auto& p:Snapshot.PocketStates)diagnostic<<"\npocket "<<unsigned(p.State)<<' '<<p.PocketId<<' '<<p.PotEventEmitted;
        Output.CandidateDiagnostic=diagnostic.str();
        // A rejected input has no valid checkpoint. Preserve it as diagnostics;
        // do not fabricate repaired positions/velocities and call that replay.
        if(!IsFinite(Output.Time) || Output.Time<0)Output.Time=0;
    }
}
bool FiniteBall(const BallState& Ball)
{
    return Ball.PositionMeters.IsFinite() && Ball.LinearVelocityMetersPerSecond.IsFinite() &&
           Ball.AngularVelocityRadiansPerSecond.IsFinite() && IsFinite(Ball.RadiusMeters) &&
           IsFinite(Ball.MassKg) && IsFinite(Ball.SleepTimeSeconds);
}
}
const char* PhysicsFaultCodeName(PhysicsFaultCode Code)
{
    switch(Code){
    case PhysicsFaultCode::None:return "None";
    case PhysicsFaultCode::NonFinite:return "NonFinite";
    case PhysicsFaultCode::Penetration:return "Penetration";
    case PhysicsFaultCode::VelocityExplosion:return "VelocityExplosion";
    case PhysicsFaultCode::Timeout:return "Timeout";
    case PhysicsFaultCode::EventLoop:return "EventLoop";
    case PhysicsFaultCode::LeftTable:return "LeftTable";
    case PhysicsFaultCode::PocketEscape:return "PocketEscape";
    case PhysicsFaultCode::SolverError:return "SolverError";
    }
    return "Unknown";
}
bool TryParsePhysicsFaultCode(const std::string& Text,PhysicsFaultCode& Output)
{
    for(const auto Code:{PhysicsFaultCode::None,PhysicsFaultCode::NonFinite,PhysicsFaultCode::Penetration,
        PhysicsFaultCode::VelocityExplosion,PhysicsFaultCode::Timeout,PhysicsFaultCode::EventLoop,
        PhysicsFaultCode::LeftTable,PhysicsFaultCode::PocketEscape,PhysicsFaultCode::SolverError})
        if(Text==PhysicsFaultCodeName(Code)){Output=Code;return true;}
    return false;
}
bool PhysicsSafetyLimits::IsValid(std::string* Error) const
{
    if(!IsFinite(MaxLinearSpeedMetersPerSecond) || MaxLinearSpeedMetersPerSecond<=0 ||
       !IsFinite(MaxSimulationTimeSeconds) || MaxSimulationTimeSeconds<=0 ||
       !IsFinite(MaxPenetrationMeters) || MaxPenetrationMeters<0 || MaxPenetrationMeters>=1 ||
       MaxEventsPerStep==0 || !IsFinite(PocketEscapeDepthMeters) || PocketEscapeDepthMeters<=0)
        return Fail(Error,"invalid physics safety limits");
    return true;
}
std::string PhysicsSafetyLimits::Serialize() const
{
    std::ostringstream Stream;Stream.imbue(std::locale::classic());
    Stream<<std::setprecision(std::numeric_limits<Scalar>::max_digits10)
          <<"snooker-safety-v1 "<<MaxLinearSpeedMetersPerSecond<<' '<<MaxSimulationTimeSeconds<<' '
          <<MaxPenetrationMeters<<' '<<MaxEventsPerStep<<' '<<PocketEscapeDepthMeters;
    return Stream.str();
}
bool PhysicsSafetyLimits::Deserialize(const std::string& Text,PhysicsSafetyLimits& Output,std::string* Error)
{
    std::istringstream Stream(Text);Stream.imbue(std::locale::classic());PhysicsSafetyLimits Candidate;std::string Magic;
    if(!(Stream>>Magic>>Candidate.MaxLinearSpeedMetersPerSecond>>Candidate.MaxSimulationTimeSeconds>>
         Candidate.MaxPenetrationMeters) || Magic!="snooker-safety-v1" ||
       !ReadUnsigned(Stream,Candidate.MaxEventsPerStep) || !(Stream>>Candidate.PocketEscapeDepthMeters))
        return Fail(Error,"invalid physics safety schema");
    Stream>>std::ws;if(!Stream.eof())return Fail(Error,"unexpected physics safety tail");
    if(!Candidate.IsValid(Error))return false;Output=Candidate;return true;
}
bool PhysicsFault::IsValid(std::string* Error) const
{
    if(Code==PhysicsFaultCode::None || std::string(PhysicsFaultCodeName(Code))=="Unknown" ||
       !IsFinite(Time) || Message.empty() || !Snapshot.IsValid(Error) ||
       Time<0 || Step<Snapshot.StepIndex)
        return Fail(Error,"invalid physics fault record");
    return true;
}
std::string PhysicsFault::Serialize() const
{
    std::ostringstream Stream;Stream.imbue(std::locale::classic());
    Stream<<std::setprecision(std::numeric_limits<Scalar>::max_digits10)
          <<"snooker-fault-v2 "<<PhysicsFaultCodeName(Code)<<' '<<Step<<' '<<Time<<' '<<Ball<<' '
          <<std::quoted(Message)<<' '<<std::quoted(Snapshot.Serialize())<<' '
          <<std::quoted(CandidateDiagnostic)<<' '<<std::quoted(ReplayConfig);
    return Stream.str();
}
bool PhysicsFault::Deserialize(const std::string& Text,PhysicsFault& Output,std::string* Error)
{
    std::istringstream Stream(Text);Stream.imbue(std::locale::classic());PhysicsFault Candidate;std::string Magic,CodeName,SnapshotText;
    if(Text.size()>32*1024*1024)return Fail(Error,"fault file exceeds 32 MiB");
    if(!(Stream>>Magic>>CodeName) || (Magic!="snooker-fault-v1" && Magic!="snooker-fault-v2") || !TryParsePhysicsFaultCode(CodeName,Candidate.Code) ||
       !ReadUnsigned(Stream,Candidate.Step) || !(Stream>>Candidate.Time) || !ReadUnsigned(Stream,Candidate.Ball) ||
       !(Stream>>std::quoted(Candidate.Message)>>std::quoted(SnapshotText)) ||
       !PhysicsSnapshot::Deserialize(SnapshotText,Candidate.Snapshot,Error))return Fail(Error,"invalid physics fault record");
    if(Magic=="snooker-fault-v2" && !(Stream>>std::quoted(Candidate.CandidateDiagnostic)>>std::quoted(Candidate.ReplayConfig)))
        return Fail(Error,"missing fault diagnostics/config");
    Stream>>std::ws;if(!Stream.eof())return Fail(Error,"unexpected physics fault tail");
    if(!Candidate.IsValid(Error))return false;Output=std::move(Candidate);return true;
}
bool CheckPhysicsSafety(const PhysicsSnapshot& Candidate,const TableGeometry& Geometry,
                        const PhysicsSafetyLimits& Limits,std::uint64_t EventsThisStep,
                        PhysicsFault& Output,std::string* Error)
{
    if(!Limits.IsValid(Error))return false;
    const auto fail=[&](PhysicsFaultCode Code,BallId Ball,const char* Message){
        SetFault(Output,Code,Candidate,Ball,Message);if(Error)*Error=Message;return false;};
    if(!IsFinite(Candidate.SimulationTimeSeconds) || !IsFinite(Candidate.FixedStepSeconds) ||
       !IsFinite(Candidate.AccumulatorSeconds))return fail(PhysicsFaultCode::NonFinite,0,"nonfinite physics clock");
    if(Candidate.SimulationTimeSeconds>Limits.MaxSimulationTimeSeconds)
        return fail(PhysicsFaultCode::Timeout,0,"physics simulation timeout");
    if(EventsThisStep>Limits.MaxEventsPerStep)return fail(PhysicsFaultCode::EventLoop,0,"physics event loop limit exceeded");
    for(const auto& ball:Candidate.Balls)if(!FiniteBall(ball))
        return fail(PhysicsFaultCode::NonFinite,ball.Id,"nonfinite ball state");
    std::string validationError;
    if(!Candidate.IsValid(&validationError)) {
        return fail(PhysicsFaultCode::SolverError,0,validationError.c_str());
    }
    for(std::size_t i=0;i<Candidate.Balls.size();++i) {
        const auto& Ball=Candidate.Balls[i];
        if(!FiniteBall(Ball))return fail(PhysicsFaultCode::NonFinite,Ball.Id,"nonfinite ball state");
        const auto speed=Ball.LinearVelocityMetersPerSecond.Length();
        if(!IsFinite(speed))return fail(PhysicsFaultCode::NonFinite,Ball.Id,"nonfinite ball speed");
        if(speed>Limits.MaxLinearSpeedMetersPerSecond)return fail(PhysicsFaultCode::VelocityExplosion,Ball.Id,"ball speed limit exceeded");
        const auto pocket=Candidate.PocketStates.empty()?PocketStateData{}:Candidate.PocketStates[i];
        if(pocket.State==PocketState::Potted)continue;
        if(pocket.State==PocketState::InPocket) {
            const PocketGeometry* geometry=nullptr;
            for(const auto& item:Geometry.Pockets)if(item.Definition.Id==pocket.PocketId){geometry=&item;break;}
            if(!geometry)return fail(PhysicsFaultCode::SolverError,Ball.Id,"in-pocket state references unknown pocket");
            if(Ball.PositionMeters.Z<geometry->BottomCenter.Z-Limits.PocketEscapeDepthMeters)
                return fail(PhysicsFaultCode::PocketEscape,Ball.Id,"ball escaped below pocket bounds");
            continue;
        }
        const auto penetration=Ball.RadiusMeters-Ball.PositionMeters.Z;
        if((pocket.State==PocketState::OnCloth || pocket.State==PocketState::Ejected) && penetration>Limits.MaxPenetrationMeters)
            return fail(PhysicsFaultCode::Penetration,Ball.Id,"ball penetrated cloth support");
        if((pocket.State==PocketState::OnCloth || pocket.State==PocketState::Ejected) && Geometry.Cloth.IsValid() &&
           (Ball.PositionMeters.X<Geometry.Cloth.Min.X || Ball.PositionMeters.X>Geometry.Cloth.Max.X ||
            Ball.PositionMeters.Y<Geometry.Cloth.Min.Y || Ball.PositionMeters.Y>Geometry.Cloth.Max.Y))
            return fail(PhysicsFaultCode::LeftTable,Ball.Id,"ball left playable cloth bounds");
        for(std::size_t j=0;j<i;++j) {
            if(!Candidate.PocketStates.empty() && Candidate.PocketStates[j].State==PocketState::Potted)continue;
            const auto& other=Candidate.Balls[j];
            if(Ball.RadiusMeters+other.RadiusMeters-(Ball.PositionMeters-other.PositionMeters).Length()>Limits.MaxPenetrationMeters)
                return fail(PhysicsFaultCode::Penetration,Ball.Id,"ball pair penetration");
        }
    }
    Output=PhysicsFault{};return true;
}
}
