#pragma once
#include <Snooker/Rules/ShotPhysicsResult.h>
#include <Snooker/Rules/MatchState.h>

namespace Snooker
{
struct SnookeredResult {bool Snookered=false;std::vector<BallId> LegalTargets;bool IsValid(std::string* Error=nullptr) const;};
bool QuerySnookered(const PhysicsSnapshot&,const std::vector<BallDefinition>&,BallId CueBall,
                    EBallType BallOn,SnookeredResult& Output,BallId ExcludedBlocker=0,
                    std::string* Error=nullptr);
bool OfferFreeBall(const MatchState&,const PhysicsSnapshot&,const std::vector<BallDefinition>&,
                   BallId CueBall,MatchState& Output,std::string* Error=nullptr);
bool NominateFreeBall(const MatchState&,EBallType,MatchState& Output,std::string* Error=nullptr);
struct FreeBallShotResult {MatchState State;bool Foul=false;std::uint32_t Points=0;std::vector<BallId> ColorsToRespot;std::string Reason;bool IsValid(std::string* Error=nullptr) const;};
bool EvaluateFreeBallShot(const MatchState&,const ShotPhysicsResult&,FreeBallShotResult& Output,
                          std::string* Error=nullptr);
}
