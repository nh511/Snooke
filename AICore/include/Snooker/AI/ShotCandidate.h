#pragma once
#include <Snooker/AI/StateQuery.h>
#include <Snooker/Table/TableGeometry.h>
#include <Snooker/Shot/ShotInput.h>

namespace Snooker
{
struct ShotCandidate
{
    BallId CueBall=0,TargetBall=0;std::uint32_t Pocket=0;
    Vec3d CueDirection{1,0,0},GhostCenter,StrikePointOnBall,CueTipOffsetMeters;
    Scalar ForwardSpeedMetersPerSecond=0,CutAngleRadians=0,Risk=0;
    bool IsValid(std::string* Error=nullptr) const;
    bool ToShotInput(const IAIStateQuery&,ShotInput& Output,std::string* Error=nullptr) const;
};
bool CreatePotCandidate(const IAIStateQuery&,const TableGeometry&,BallId Target,std::uint32_t Pocket,
                        Scalar Speed,ShotCandidate& Output,std::string* Error=nullptr);
}
