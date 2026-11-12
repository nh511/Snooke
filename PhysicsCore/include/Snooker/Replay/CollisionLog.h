#pragma once
#include <Snooker/Physics/BallResponse.h>
#include <vector>

namespace Snooker
{
struct CollisionEvent
{
    StepIndex Step=0;
    Seconds Time=0;
    BallState FirstBefore, SecondBefore, FirstAfter, SecondAfter;
    BallCollisionResponse Response;
};
struct CollisionLog
{
    BallCollisionProfile Parameters;
    std::vector<CollisionEvent> Events;
    bool IsValid(std::string* Error=nullptr) const;
    // Only applied impulses append events; a separating contact is a successful no-op.
    // All numerical/validation failures preserve balls and log.
    bool ResolveAndAppend(BallState& First,BallState& Second,StepIndex Step,Seconds Time,std::string* Error=nullptr);
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,CollisionLog& Output,std::string* Error=nullptr);
};
}
