#pragma once
#include <Snooker/Rules/FrameOpening.h>

namespace Snooker
{
struct FrameEndResult
{
    MatchState State;
    bool FrameEnded=false,NeedsRespottedBlack=false;
    std::uint32_t Winner=0;
    bool IsValid(std::string* Error=nullptr) const;
};
// RespottedBlackStarter is an explicit deterministic draw/result supplied by the caller.
bool ResolveFrameEnd(const MatchState&,std::uint32_t RespottedBlackStarter,
                     FrameEndResult& Output,std::string* Error=nullptr);
bool StartNextFrame(const MatchState& Finished,const TableDefinition&,const BallDefinition& Prototype,
                    const OpeningLayout&,const std::string& PhysicsVersion,Seconds FixedStep,
                    std::uint32_t Breaker,FrameOpening& Output,std::string* Error=nullptr);
}
