#pragma once
#include <Snooker/Service/PocketSimulation.h>
#include <Snooker/Shot/ShotInput.h>
#include <memory>

namespace Snooker
{
using PhysicsBundle=PocketSimulationConfig;
enum class PhysicsRunState { Running, Settled, Faulted };
enum class PhysicsEventKind { CueStrike, BallBall, Cushion, Jaw, PocketContact, MouthEntered,
    Potted, RecoveredToCloth, BallSettled, ShotSettled };
struct PhysicsEvent
{
    std::uint64_t Sequence=0;
    StepIndex Step=0;
    Seconds Time=0;
    PhysicsEventKind Kind=PhysicsEventKind::CueStrike;
    BallId Ball=0, Other=0;
    Vec3d Point, Normal;
    Scalar Impulse=0;
    std::string Serialize() const;
};
class ISnookerPhysicsService
{
public:
    virtual ~ISnookerPhysicsService()=default;
    virtual bool BeginShot(const PhysicsSnapshot&,const ShotInput&,std::string* Error=nullptr)=0;
    // Invalid frame time is transactional. A physical fault latches Faulted,
    // retaining the last completed fixed step and its events.
    virtual bool Step(Seconds FrameTime,PhysicsRunState& Output,std::string* Error=nullptr)=0;
    virtual bool IsSettled() const=0;
    virtual const std::vector<PhysicsEvent>& GetEvents() const=0;
    virtual PhysicsSnapshot SaveSnapshot() const=0;
    virtual bool RestoreSnapshot(const PhysicsSnapshot&,std::string* Error=nullptr)=0;
    virtual const PhysicsFault& GetFault() const=0;
};
std::unique_ptr<ISnookerPhysicsService> CreatePhysicsService(const PhysicsBundle&,std::string* Error=nullptr);
}
