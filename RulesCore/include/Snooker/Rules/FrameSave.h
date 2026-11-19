#pragma once
#include <Snooker/Replay/PhysicsShotRecord.h>
#include <Snooker/Rules/MatchState.h>

namespace Snooker
{
struct FrameSave
{
    std::string GameVersion="standalone-game-v1";
    PhysicsBundle Parameters;
    MatchState Match;
    PhysicsSnapshot Physics;
    std::vector<BallDefinition> Definitions;
    std::vector<PhysicsShotRecord> Shots;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string&,FrameSave& Output,std::string* Error=nullptr);
};
bool ContinueFrame(const FrameSave&,const ShotInput&,FrameSave& Output,
                   PhysicsShotRecord* Shot=nullptr,std::string* Error=nullptr);
}
