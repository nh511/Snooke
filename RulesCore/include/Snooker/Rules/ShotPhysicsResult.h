#pragma once
#include <Snooker/Replay/PhysicsShotRecord.h>

namespace Snooker
{
struct ShotPhysicsResult
{
    BallId CueBall=0;
    std::vector<BallDefinition> Definitions;
    std::vector<PhysicsEvent> Events;
    PhysicsSnapshot Initial,Final;
    // Zero denotes no ball-ball contact by the cue ball during this shot.
    BallId FirstContact=0;
    std::vector<BallId> Potted;
    bool CueBallPotted=false;
    bool IsValid(std::string* Error=nullptr) const;
};
// Ball IDs do not encode types. Metadata is explicit, checked against immutable
// ball radius/mass/material, and never inferred from render/Actor state.
bool BuildShotRuleInput(const PhysicsShotRecord&,const std::vector<BallDefinition>&,
                       ShotPhysicsResult& Output,std::string* Error=nullptr);
}
