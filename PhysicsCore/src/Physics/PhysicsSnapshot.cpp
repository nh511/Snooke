#include "Snooker/Physics/PhysicsSnapshot.h"

#include <iomanip>
#include <locale>
#include <Snooker/Serialization/Text.h>
#include <sstream>

namespace Snooker
{
namespace
{
const char* PhaseToString(const ESnapshotPhase Phase)
{
    switch (Phase)
    {
    case ESnapshotPhase::BeforeShot: return "BeforeShot";
    case ESnapshotPhase::DuringStep: return "DuringStep";
    case ESnapshotPhase::AfterShot: return "AfterShot";
    }
    return "Unknown";
}

bool TryParsePhase(const std::string& Value, ESnapshotPhase& OutPhase)
{
    for (const ESnapshotPhase Candidate : {ESnapshotPhase::BeforeShot, ESnapshotPhase::DuringStep, ESnapshotPhase::AfterShot})
    {
        if (Value == PhaseToString(Candidate))
        {
            OutPhase = Candidate;
            return true;
        }
    }
    return false;
}

void SetError(std::string* OutError, const char* Message)
{
    if (OutError != nullptr)
    {
        *OutError = Message;
    }
}
}

bool FPhysicsSnapshot::IsValid(std::string* OutError) const
{
    if (std::string(PhaseToString(Phase)) == "Unknown" || Balls.size() > MaxSerializedBalls)
    {
        SetError(OutError, "invalid snapshot phase or ball count");
        return false;
    }
    if (SchemaVersion != CurrentSchemaVersion || (!PocketStates.empty() && PocketStates.size()!=Balls.size()))
    {
        SetError(OutError, "unsupported physics snapshot schema version");
        return false;
    }
    if (PhysicsParameterVersion.empty() || PhysicsParameterVersion.find_first_of(" \t\r\n") != std::string::npos)
    {
        SetError(OutError, "physics parameter version must be a non-empty token");
        return false;
    }
    if (!IsFinite(SimulationTimeSeconds) || SimulationTimeSeconds < 0.0)
    {
        SetError(OutError, "simulation time must be finite and non-negative");
        return false;
    }
    PhysicsClock Clock;
    if (!Clock.Restore({FixedStepSeconds, AccumulatorSeconds, StepIndex}) ||
        !NearlyEqual(SimulationTimeSeconds, Clock.SimulationTime()))
    {
        SetError(OutError, "invalid or inconsistent clock state");
        return false;
    }
    for (std::size_t Index = 0; Index < Balls.size(); ++Index)
    {
        if (!Balls[Index].IsValid(OutError))
        {
            return false;
        }
        if(!PocketStates.empty()) {
            const auto& p=PocketStates[Index];
            if(static_cast<unsigned>(p.State)>4 || (p.State==PocketState::Potted)!=p.PotEventEmitted ||
               ((p.State==PocketState::InPocket || p.State==PocketState::Potted) && p.PocketId==0) ||
               ((p.State==PocketState::OnCloth || p.State==PocketState::Ejected) && p.PocketId!=0) ||
               (p.State==PocketState::Potted && (!Balls[Index].Sleeping ||
                 Balls[Index].LinearVelocityMetersPerSecond.LengthSquared()!=0 ||
                 Balls[Index].AngularVelocityRadiansPerSecond.LengthSquared()!=0))) {
                SetError(OutError,"inconsistent snapshot pocket state");return false;
            }
        }
        for (std::size_t OtherIndex = 0; OtherIndex < Index; ++OtherIndex)
        {
            if (Balls[Index].Id == Balls[OtherIndex].Id)
            {
                SetError(OutError, "physics snapshot contains duplicate ball ids");
                return false;
            }
        }
    }
    return true;
}

bool FPhysicsSnapshot::operator==(const FPhysicsSnapshot& Other) const
{
    if (SchemaVersion != Other.SchemaVersion || Phase != Other.Phase ||
        PhysicsParameterVersion != Other.PhysicsParameterVersion || StepIndex != Other.StepIndex ||
        !NearlyEqual(SimulationTimeSeconds, Other.SimulationTimeSeconds) ||
        !NearlyEqual(FixedStepSeconds, Other.FixedStepSeconds) ||
        !NearlyEqual(AccumulatorSeconds, Other.AccumulatorSeconds) || Balls.size() != Other.Balls.size() ||
        EventCount!=Other.EventCount)
    {
        return false;
    }
    for (std::size_t Index = 0; Index < Balls.size(); ++Index)
    {
        if (!(Balls[Index] == Other.Balls[Index]))
        {
            return false;
        }
        const auto a=PocketStates.empty()?PocketStateData{}:PocketStates[Index];
        const auto b=Other.PocketStates.empty()?PocketStateData{}:Other.PocketStates[Index];
        if(a.State!=b.State || a.PocketId!=b.PocketId || a.PotEventEmitted!=b.PotEventEmitted)return false;
    }
    return true;
}

std::string FPhysicsSnapshot::Serialize() const
{
    std::ostringstream Stream;
    Stream.imbue(std::locale::classic());
    Stream << std::setprecision(std::numeric_limits<Scalar>::max_digits10)
           << SchemaVersion << ' ' << PhaseToString(Phase) << ' ' << PhysicsParameterVersion << ' ' << StepIndex << ' '
           << SimulationTimeSeconds << ' ' << FixedStepSeconds << ' ' << AccumulatorSeconds << ' ' << Balls.size()
           << ' ' << EventCount;
    for (std::size_t i=0;i<Balls.size();++i)
    {
        const auto p=PocketStates.empty()?PocketStateData{}:PocketStates[i];
        Stream << '\n' << std::quoted(Balls[i].Serialize()) << ' ' << static_cast<unsigned>(p.State)
               << ' ' << p.PocketId << ' ' << unsigned(p.PotEventEmitted);
    }
    return Stream.str();
}

bool FPhysicsSnapshot::Deserialize(const std::string& Serialized, FPhysicsSnapshot& OutSnapshot, std::string* OutError)
{
    std::istringstream Stream(Serialized);
    Stream.imbue(std::locale::classic());
    FPhysicsSnapshot Candidate;
    std::string PhaseName;
    std::size_t BallCount = 0;
    if (!(ReadUnsigned(Stream, Candidate.SchemaVersion) && Stream >> PhaseName >> Candidate.PhysicsParameterVersion >> std::ws) || !ReadUnsigned(Stream, Candidate.StepIndex) || !(Stream >>
          Candidate.SimulationTimeSeconds >> Candidate.FixedStepSeconds >> Candidate.AccumulatorSeconds) || !ReadUnsigned(Stream, BallCount) || !TryParsePhase(PhaseName, Candidate.Phase))
    {
        SetError(OutError, "invalid physics snapshot header");
        return false;
    }
    const bool legacy=Candidate.SchemaVersion==2;
    if(!legacy && (Candidate.SchemaVersion!=CurrentSchemaVersion || !ReadUnsigned(Stream,Candidate.EventCount))) {
        SetError(OutError,"unsupported or invalid snapshot schema");return false;
    }
    Candidate.SchemaVersion=CurrentSchemaVersion;
    std::string Line;
    std::getline(Stream, Line);
    if (BallCount > MaxSerializedBalls || Line.find_first_not_of(" \t\r") != std::string::npos)
    {
        SetError(OutError, "invalid snapshot count or trailing header data");
        return false;
    }
    Candidate.Balls.reserve(BallCount);
    for (std::size_t Index = 0; Index < BallCount; ++Index)
    {
        if (!std::getline(Stream, Line))
        {
            SetError(OutError, "physics snapshot ended before all balls were read");
            return false;
        }
        FBallState Ball;
        PocketStateData pocket;
        std::string ballText=Line;
        if(!legacy) {
            std::istringstream row(Line);row.imbue(std::locale::classic());
            unsigned state=0,emitted=0;
            if(!(row>>std::quoted(ballText)) || !ReadUnsigned(row,state) || !ReadUnsigned(row,pocket.PocketId) ||
               !ReadUnsigned(row,emitted) || state>4 || emitted>1 || !(row>>std::ws).eof()) {
                SetError(OutError,"invalid snapshot pocket row");return false;
            }
            pocket.State=static_cast<PocketState>(state);pocket.PotEventEmitted=emitted!=0;
        }
        if (!FBallState::Deserialize(ballText, Ball, OutError))
        {
            return false;
        }
        Candidate.Balls.push_back(Ball);
        Candidate.PocketStates.push_back(pocket);
    }
    Stream >> std::ws;
    if (!Stream.eof())
    {
        SetError(OutError, "unexpected data after physics snapshot");
        return false;
    }
    if (!Candidate.IsValid(OutError))
    {
        return false;
    }
    OutSnapshot = Candidate;
    return true;
}

PhysicsSnapshot PhysicsSnapshot::Capture(const PhysicsClock& Clock, const std::vector<BallState>& Balls,
                                        const std::string& Version, ESnapshotPhase Phase)
{
    PhysicsSnapshot Result;
    Result.Phase=Phase; Result.PhysicsParameterVersion=Version;
    Result.StepIndex=Clock.State().CompletedSteps; Result.SimulationTimeSeconds=Clock.SimulationTime();
    Result.FixedStepSeconds=Clock.State().FixedStep; Result.AccumulatorSeconds=Clock.State().Accumulator;
    Result.Balls=Balls;
    Result.PocketStates.resize(Balls.size());
    return Result;
}
bool PhysicsSnapshot::Restore(PhysicsClock& Clock, std::vector<BallState>& OutputBalls,
                              const std::string& ExpectedParameterVersion, std::string* Error) const
{
    if (!IsValid(Error)) return false;
    // Legacy clock/balls API must not silently discard service-owned state.
    for(const auto& p:PocketStates)if(p.State!=PocketState::OnCloth || p.PocketId || p.PotEventEmitted) {
        SetError(Error,"use a pocket-aware service to restore pocket state");return false;
    }
    if(EventCount) {SetError(Error,"use a service to restore the event cursor");return false;}
    if (PhysicsParameterVersion!=ExpectedParameterVersion)
    {
        SetError(Error,"snapshot parameter version does not match loaded configuration");
        return false;
    }
    PhysicsClock Candidate;
    if (!Candidate.Restore({FixedStepSeconds,AccumulatorSeconds,StepIndex})) return false;
    auto CopiedBalls=this->Balls;
    OutputBalls.swap(CopiedBalls);
    Clock=Candidate;
    return true;
}
} // namespace Snooker
