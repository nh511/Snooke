#pragma once
#include <Snooker/Data/CoreConfig.h>
#include <Snooker/Physics/PhysicsSnapshot.h>
#include <Snooker/Shot/ShotInput.h>
#include <vector>
#include <string>

namespace Snooker
{
struct ShotEventRecord
{
    std::uint64_t Step=0;
    Seconds Time=0;
    std::string Kind;
    BallId Ball=0, OtherBall=0;
    Vec3d Point, Normal;
};
struct ShotRecord
{
    std::string BuildVersion;
    CoreConfig Parameters;
    PhysicsSnapshot Initial, Final;
    FShotInput Input;
    std::vector<ShotEventRecord> Events;
    std::string Result;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text, ShotRecord& Output, std::string* Error=nullptr);
};
}
