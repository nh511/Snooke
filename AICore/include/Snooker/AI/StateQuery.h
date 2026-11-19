#pragma once
#include <Snooker/Rules/BallOn.h>
#include <Snooker/Physics/PhysicsSnapshot.h>
#include <Snooker/Shot/CueProfile.h>
#include <memory>

namespace Snooker
{
class IAIStateQuery
{
public:
    virtual ~IAIStateQuery()=default;
    virtual const MatchState& Match() const=0;
    virtual const PhysicsSnapshot& Physics() const=0;
    virtual const std::vector<BallDefinition>& Definitions() const=0;
    virtual const CueProfile& Cue() const=0;
    virtual const BallOnQuery& BallOn() const=0;
    virtual const BallState* FindBall(BallId) const=0;
    virtual std::uint64_t StateHash() const=0;
};
std::unique_ptr<IAIStateQuery> CreateAIStateQuery(const MatchState&,const PhysicsSnapshot&,
    const std::vector<BallDefinition>&,const CueProfile&,std::string* Error=nullptr);
}
