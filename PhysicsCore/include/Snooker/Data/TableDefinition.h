#pragma once
#include <Snooker/Data/BallDefinition.h>
#include <Snooker/Math/CoreTypes.h>
#include <vector>
#include <string>

namespace Snooker
{
struct TableBounds
{
    Vec2d Min, Max;
    bool IsValid() const;
    bool Contains(const Vec2d& Point) const;
};
struct CushionDefinition
{
    std::uint32_t Id=0;
    Vec3d Start, End;
    std::string ProfileId;
};
struct PocketDefinition
{
    std::uint32_t Id=0;
    Vec3d Center;
    Meters Opening=0, Depth=0;
    std::string ProfileId;
};
struct BallSpot { EBallType Type=EBallType::Black; Vec3d Position; };
struct TableDefinition
{
    std::string ParameterVersion;
    TableBounds Table, Cloth;
    std::vector<CushionDefinition> Cushions;
    std::vector<PocketDefinition> Pockets;
    std::vector<BallSpot> Spots;
    std::vector<BallDefinition> InitialBalls;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text, TableDefinition& Output, std::string* Error=nullptr);
};
}
