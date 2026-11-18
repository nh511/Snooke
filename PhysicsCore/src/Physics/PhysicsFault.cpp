#include <Snooker/Physics/PhysicsFault.h>
#include <Snooker/Serialization/Text.h>
#include <iomanip>
#include <locale>
#include <sstream>
#include <set>

namespace Snooker
{
namespace
{
bool Fail(std::string* Error,const char* Message){if(Error)*Error=Message;return false;}
bool Token(const std::string& Value){return !Value.empty() && Value.find_first_of(" \t\r\n")==std::string::npos;}
void SetFault(PhysicsFault& Output,PhysicsFaultCode Code,const PhysicsSnapshot& Snapshot,
              BallId Ball,const std::string& Message)
{
    Output.Code=Code;Output.Step=Snapshot.StepIndex;Output.Time=Snapshot.SimulationTimeSeconds;
    Output.Ball=Ball;Output.Message=Message;Output.Snapshot=Snapshot;
}
bool FiniteBall(const BallState& Ball)
{
    return Ball.PositionMeters.IsFinite() && Ball.LinearVelocityMetersPerSecond.IsFinite() &&
           Ball.AngularVelocityRadiansPerSecond.IsFinite() && IsFinite(Ball.RadiusMeters) &&
           IsFinite(Ball.MassKg) && IsFinite(Ball.SleepTimeSeconds);
}
PhysicsSnapshot RecoverableSnapshot(const PhysicsSnapshot& Source)
{
    auto Result=Source;Result.SchemaVersion=PhysicsSnapshot::CurrentSchemaVersion;
    if(!IsFinite(Result.FixedStepSeconds) || Result.FixedStepSeconds<1e-6 || Result.FixedStepSeconds>1)
        Result.FixedStepSeconds=1.0/240;
    if(!IsFinite(Result.AccumulatorSeconds) || Result.AccumulatorSeconds<0)Result.AccumulatorSeconds=0;
    if(!IsFinite(Result.SimulationTimeSeconds) || Result.SimulationTimeSeconds<0)Result.SimulationTimeSeconds=0;
    const auto calculated=Result.SimulationTimeSeconds/Result.FixedStepSeconds;
    if(!IsFinite(calculated) || calculated<0 || calculated>static_cast<Scalar>(std::numeric_limits<std::uint64_t>::max()))
        Result.StepIndex=0;
    else Result.StepIndex=std::max(Result.StepIndex,static_cast<std::uint64_t>(calculated));
    Result.SimulationTimeSeconds=Result.StepIndex*Result.FixedStepSeconds;
    Result.AccumulatorSeconds=0;Result.Phase=ESnapshotPhase::DuringStep;
    std::set<BallId> ids;
    for(auto& Ball:Result.Balls) {
        if(!ids.insert(Ball.Id).second || Ball.Id==0)Ball.Id=static_cast<BallId>(ids.size()+1),ids.insert(Ball.Id);
        if(!Ball.PositionMeters.IsFinite())Ball.PositionMeters={};
        if(!Ball.LinearVelocityMetersPerSecond.IsFinite())Ball.LinearVelocityMetersPerSecond={};
        if(!Ball.AngularVelocityRadiansPerSecond.IsFinite())Ball.AngularVelocityRadiansPerSecond={};
        if(!IsFinite(Ball.RadiusMeters) || Ball.RadiusMeters<=0)Ball.RadiusMeters=0.02625;
        if(!IsFinite(Ball.MassKg) || Ball.MassKg<=0)Ball.MassKg=0.142;
        if(!IsFinite(Ball.SleepTimeSeconds) || Ball.SleepTimeSeconds<0)Ball.SleepTimeSeconds=0;
        if(Ball.MaterialProfileId.empty() || !Token(Ball.MaterialProfileId))Ball.MaterialProfileId="standard";
        Ball.Contact={EContactKind::None,0,TableNormal};
    }
    Result.PocketStates.resize(Result.Balls.size());
    for(auto& State:Result.PocketStates)State={PocketState::OnCloth,0,false};
    Result.EventCount=0;
    return Result;
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
       Step!=Snapshot.StepIndex || !NearlyEqual(Time,Snapshot.SimulationTimeSeconds))
        return Fail(Error,"invalid physics fault record");
    return true;
}
std::string PhysicsFault::Serialize() const
{
    std::ostringstream Stream;Stream.imbue(std::locale::classic());
    Stream<<std::setprecision(std::numeric_limits<Scalar>::max_digits10)
          <<"snooker-fault-v1 "<<PhysicsFaultCodeName(Code)<<' '<<Step<<' '<<Time<<' '<<Ball<<' '
          <<std::quoted(Message)<<' '<<std::quoted(Snapshot.Serialize());
    return Stream.str();
}
bool PhysicsFault::Deserialize(const std::string& Text,PhysicsFault& Output,std::string* Error)
{
    std::istringstream Stream(Text);Stream.imbue(std::locale::classic());PhysicsFault Candidate;std::string Magic,CodeName,SnapshotText;
    if(!(Stream>>Magic>>CodeName) || Magic!="snooker-fault-v1" || !TryParsePhysicsFaultCode(CodeName,Candidate.Code) ||
       !ReadUnsigned(Stream,Candidate.Step) || !(Stream>>Candidate.Time) || !ReadUnsigned(Stream,Candidate.Ball) ||
       !(Stream>>std::quoted(Candidate.Message)>>std::quoted(SnapshotText)) ||
       !PhysicsSnapshot::Deserialize(SnapshotText,Candidate.Snapshot,Error))return Fail(Error,"invalid physics fault record");
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
    if(!Candidate.IsValid(Error)) {
        const auto message=Error?*Error:"invalid physics snapshot";
        const auto code=message.find("finite")!=std::string::npos?PhysicsFaultCode::NonFinite:PhysicsFaultCode::SolverError;
        const auto safe=RecoverableSnapshot(Candidate);SetFault(Output,code,safe,0,message);if(Error)*Error=message;return false;
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
        if(penetration>Limits.MaxPenetrationMeters)return fail(PhysicsFaultCode::Penetration,Ball.Id,"ball penetrated cloth support");
        if((pocket.State==PocketState::OnCloth || pocket.State==PocketState::Ejected) && Geometry.Cloth.IsValid() &&
           (Ball.PositionMeters.X-Ball.RadiusMeters<Geometry.Cloth.Min.X ||
           Ball.PositionMeters.X+Ball.RadiusMeters>Geometry.Cloth.Max.X ||
           Ball.PositionMeters.Y-Ball.RadiusMeters<Geometry.Cloth.Min.Y ||
           Ball.PositionMeters.Y+Ball.RadiusMeters>Geometry.Cloth.Max.Y))
            return fail(PhysicsFaultCode::LeftTable,Ball.Id,"ball left playable cloth bounds");
    }
    Output=PhysicsFault{};return true;
}
}
