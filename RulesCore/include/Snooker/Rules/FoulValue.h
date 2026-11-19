#pragma once
#include <Snooker/Rules/FirstContact.h>
#include <Snooker/Rules/CueBallFoul.h>

namespace Snooker
{
enum class FoulReason { NoFirstContact, WrongFirstContact, CueBallPotted, IllegalBallPotted, MultipleReasons };
struct FoulValueResult
{
    bool Foul=false; FoulReason Reason=FoulReason::NoFirstContact; std::uint32_t Points=0; std::uint32_t AwardedToPlayer=0; int RelevantValue=0; std::string Explanation;
    bool IsValid(std::string* Error=nullptr) const;
};
bool CalculateFoulValue(const MatchState&,const ShotPhysicsResult&,const FirstContactResult&,const CueBallFoulResult&,FoulValueResult&,std::string* Error=nullptr);
}
