#pragma once
#include <Snooker/Rules/MatchState.h>
#include <Snooker/Table/BallPlacement.h>
#include <Snooker/Physics/PhysicsSnapshot.h>

namespace Snooker
{
struct FrameOpening
{
    MatchState Match;
    std::vector<BallDefinition> Definitions;
    PhysicsSnapshot Physics;
    std::string TableVersion, LayoutVersion;
};
bool CreateFrameOpening(const TableDefinition& Table,const BallDefinition& Prototype,const OpeningLayout& Layout,
    const std::string& PhysicsParameterVersion,Seconds FixedStep,std::uint32_t FrameNumber,std::uint32_t Breaker,
    FrameOpening& Output,std::string* Error=nullptr);
}
