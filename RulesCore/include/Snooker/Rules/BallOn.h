#pragma once
#include <Snooker/Rules/MatchState.h>
#include <vector>

namespace Snooker
{
struct BallOnQuery
{
    bool FrameFinished=false;
    EBallType Selected=EBallType::Red;
    std::vector<EBallType> LegalChoices;
};
// ColorAfterRed exposes six choices and the explicit currently nominated color.
// FinalColors exposes only the state cursor. FreeBall needs RUL-013, not guessed legality.
bool GetBallOn(const MatchState& State,BallOnQuery& Output,std::string* Error=nullptr);
}
