#pragma once
#include <Snooker/Physics/PhysicsSnapshot.h>
#include <Snooker/Data/ClothProfile.h>
#include <Snooker/Physics/BallResponse.h>
#include <Snooker/Shot/CueProfile.h>
#include <Snooker/Physics/PhysicsFault.h>

namespace Snooker
{
// Single-ball laboratory service. Ball/rail scheduling belongs to PHY-021.
struct PocketSimulationConfig
{
    std::string Version="pocket-simulation-v1";
    TableDefinition Table;
    std::vector<CushionProfile> Cushions;
    std::vector<PocketProfile> Pockets;
    BallCollisionProfile BallCollision{"bundle-ball-contact-v1",0.95,0.05,1e-7};
    CueProfile Cue;
    GeometrySettings Geometry;
    ClothProfile Cloth{"cloth-v1","engineering",0.2,0.01,0.5,0.001,0.01,0.00001,0.25};
    Seconds FixedStep=1.0/240;
    Scalar Gravity=9.80665;
    std::uint32_t Substeps=8;
    PhysicsSafetyLimits Safety;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,PocketSimulationConfig& Output,std::string* Error=nullptr);
};
enum class PocketSimulationEventKind { Contact, MouthEntered, Potted, RecoveredToCloth };
struct PocketSimulationEvent
{
    std::uint64_t Sequence=0, Step=0;
    Seconds Time=0;
    BallId Ball=0;
    std::uint32_t Pocket=0;
    PocketSimulationEventKind Kind=PocketSimulationEventKind::Contact;
    PocketFeature Feature=PocketFeature::Jaw;
    Vec3d Point, Normal;
    Scalar NormalSpeedBefore=0, NormalSpeedAfter=0;
    std::string Serialize() const;
};
class PocketSimulation
{
public:
    // Restore clears delivered events, but preserves the sequence cursor from the snapshot.
    bool Restore(const PocketSimulationConfig& Config,const PhysicsSnapshot& Snapshot,std::string* Error=nullptr);
    // One complete fixed step; failure leaves snapshot and events unchanged.
    bool Step(std::string* Error=nullptr);
    const PhysicsSnapshot& SaveSnapshot() const { return State; }
    const std::vector<PocketSimulationEvent>& GetEvents() const { return Events; }
    bool IsFaulted() const { return Fault.Code!=PhysicsFaultCode::None; }
    const PhysicsFault& GetFault() const { return Fault; }
    bool SaveFault(const std::string& Path,std::string* Error=nullptr) const;
private:
    bool StepInternal(std::string* Error);
    PocketSimulationConfig Config;
    TableGeometry Geometry;
    PhysicsSnapshot State;
    std::vector<PocketSimulationEvent> Events;
    PhysicsFault Fault;
    bool Ready=false;
};
}
