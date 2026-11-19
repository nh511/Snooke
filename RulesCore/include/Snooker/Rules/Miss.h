#pragma once
#include <Snooker/Rules/FirstContact.h>

namespace Snooker
{
struct RulesProfile {std::string Id="match";bool MissEnabled=true;std::uint32_t MinimumFoul=4;Meters DLineX=-1.04,DRadius=.292;bool IsValid(std::string* Error=nullptr) const;std::string Serialize() const;static bool Deserialize(const std::string&,RulesProfile& Output,std::string* Error=nullptr);};
struct MissResult {MatchState State;bool Foul=false,Miss=false,ClearFullBallAvailable=false,NeedsSnookers=false,CanRestore=false;std::string Reason;bool IsValid(std::string* Error=nullptr) const;};
bool EvaluateMiss(const MatchState&,const ShotPhysicsResult&,const FirstContactResult&,const RulesProfile&,MissResult& Output,std::string* Error=nullptr);
enum class MissChoice { PlayFromCurrent, RestoreAndReplay };
struct MissResolution {MatchState State;PhysicsSnapshot Physics;bool Restored=false;std::uint32_t PlayerToPlay=0;bool IsValid(std::string* Error=nullptr) const;};
bool ResolveMiss(const MissResult&,MissChoice,const PhysicsSnapshot& Before,const PhysicsSnapshot& Current,MissResolution& Output,std::string* Error=nullptr);
}
