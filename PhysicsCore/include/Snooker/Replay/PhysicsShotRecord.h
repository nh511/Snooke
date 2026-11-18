#pragma once
#include <Snooker/Service/ISnookerPhysicsService.h>

namespace Snooker
{
struct PhysicsShotRecord
{
    std::string BuildVersion="standalone-solver-v1";
    PhysicsBundle Parameters;
    PhysicsSnapshot Initial,Final;
    ShotInput Input;
    std::vector<PhysicsEvent> Events;
    std::string Result="Settled";
    PhysicsFault Fault;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string&,PhysicsShotRecord&,std::string* Error=nullptr);
};
}
