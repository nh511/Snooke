#pragma once

#include "Snooker/Physics/BallState.h"
#include "Snooker/Physics/PhysicsClock.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Snooker
{
enum class ESnapshotPhase : std::uint8_t
{
    BeforeShot,
    DuringStep,
    AfterShot,
};

struct PhysicsSnapshot
{
    static constexpr std::uint32_t CurrentSchemaVersion = 2;
    // Resource safety limit for text loading, not a snooker rules limit.
    static constexpr std::size_t MaxSerializedBalls = 4096;

    std::uint32_t SchemaVersion = CurrentSchemaVersion;
    ESnapshotPhase Phase = ESnapshotPhase::BeforeShot;
    std::string PhysicsParameterVersion = "unversioned";
    std::uint64_t StepIndex = 0;
    Seconds SimulationTimeSeconds = 0.0;
    Seconds FixedStepSeconds = 1.0/240;
    Seconds AccumulatorSeconds = 0.0;
    std::vector<FBallState> Balls;

    bool IsValid(std::string* OutError = nullptr) const;
    bool operator==(const PhysicsSnapshot& Other) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Serialized, PhysicsSnapshot& OutSnapshot, std::string* OutError = nullptr);
    static PhysicsSnapshot Capture(const PhysicsClock& Clock, const std::vector<BallState>& Balls,
                                   const std::string& ParameterVersion, ESnapshotPhase Phase);
    bool Restore(PhysicsClock& Clock, std::vector<BallState>& Balls,
                 const std::string& ExpectedParameterVersion, std::string* Error=nullptr) const;
};
using FPhysicsSnapshot = PhysicsSnapshot;
} // namespace Snooker
