#pragma once
#include <Snooker/Rules/ShotPhysicsResult.h>
#include <Snooker/Table/TableGeometry.h>

namespace Snooker
{
struct CueBallPlacementRules
{
    Meters DLineX=-1.04;
    Meters DCenterY=0;
    Meters DRadius=0.292;
    Meters Tolerance=1e-9;
    bool IsValid(std::string* Error=nullptr) const;
};
struct CueBallFoulResult
{
    bool Foul=false;
    bool CueBallInHand=false;
    bool SuppressObjectScoring=false;
    BallId CueBall=0;
    std::vector<BallId> ObjectBallsPotted;
    std::string Reason;
    bool IsValid(std::string* Error=nullptr) const;
};
// Rule-only projection. It does not assign the later RUL-008 foul value.
bool EvaluateCueBallPotted(const ShotPhysicsResult& Input,CueBallFoulResult& Output,
                           std::string* Error=nullptr);
// Creates a legal cue-ball-in-hand definition. No UE position/actor state is used.
bool PlaceCueBallInD(const TableDefinition& Table,const BallDefinition& Cue,
                     const Vec3d& Position,const std::vector<BallDefinition>& Occupied,
                     const CueBallPlacementRules& Rules,BallDefinition& Output,
                     std::string* Error=nullptr);
}
