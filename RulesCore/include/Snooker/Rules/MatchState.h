#pragma once
#include <Snooker/Data/BallDefinition.h>
#include <array>
#include <string>

namespace Snooker
{
enum class FramePhase { Reds, ColorAfterRed, FinalColors, Finished };
const char* FramePhaseName(FramePhase Phase);
struct MatchState
{
    std::uint32_t FrameNumber=1;
    FramePhase Phase=FramePhase::Reds;
    std::uint32_t CurrentPlayer=0;
    std::array<std::uint32_t,2> Scores{0,0};
    EBallType BallOn=EBallType::Red;
    std::uint32_t RemainingReds=15;
    bool CueBallInHand=false;
    bool FreeBallAvailable=false;
    bool MissPending=false;
    bool ShotInProgress=false;

    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,MatchState& Output,std::string* Error=nullptr);
};
}
