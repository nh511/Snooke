#pragma once

#include <Snooker/Physics/PhysicsSnapshot.h>
#include <Snooker/Table/TableGeometry.h>

namespace Snooker
{
enum class PhysicsFaultCode : std::uint8_t
{
    None,
    NonFinite,
    Penetration,
    VelocityExplosion,
    Timeout,
    EventLoop,
    LeftTable,
    PocketEscape,
    SolverError,
};

const char* PhysicsFaultCodeName(PhysicsFaultCode Code);
bool TryParsePhysicsFaultCode(const std::string& Text,PhysicsFaultCode& Output);

struct PhysicsSafetyLimits
{
    Scalar MaxLinearSpeedMetersPerSecond=50;
    Seconds MaxSimulationTimeSeconds=120;
    Meters MaxPenetrationMeters=1e-6;
    std::uint32_t MaxEventsPerStep=256;
    Meters PocketEscapeDepthMeters=0.0525;

    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,PhysicsSafetyLimits& Output,std::string* Error=nullptr);
};

struct PhysicsFault
{
    PhysicsFaultCode Code=PhysicsFaultCode::None;
    StepIndex Step=0;
    Seconds Time=0;
    BallId Ball=0;
    std::string Message;
    PhysicsSnapshot Snapshot;

    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,PhysicsFault& Output,std::string* Error=nullptr);
};

// Returns true when the candidate is safe. A false return is a detected fault,
// not a parser error; Output contains a reproducible snapshot and code.
bool CheckPhysicsSafety(const PhysicsSnapshot& Candidate,const TableGeometry& Geometry,
                        const PhysicsSafetyLimits& Limits,std::uint64_t EventsThisStep,
                        PhysicsFault& Output,std::string* Error=nullptr);
}
