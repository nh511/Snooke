#pragma once
#include <Snooker/Table/BallPlacement.h>

namespace Snooker
{
enum class RespotStatus { OwnSpot, AlternateSpot, NoSpotAvailable };
struct ColorRespotResult
{
    RespotStatus Status=RespotStatus::OwnSpot;
    BallDefinition Ball;
    EBallType SpotType=EBallType::Yellow;
    bool IsValid(std::string* Error=nullptr) const;
};
bool FindColorRespot(const TableDefinition&,const BallDefinition& Color,
                     const std::vector<BallDefinition>& Occupied,
                     ColorRespotResult& Output,std::string* Error=nullptr);
// Resolves multiple colors high-value first, updating occupied positions locally.
bool FindColorRespots(const TableDefinition&,const std::vector<BallDefinition>& Colors,
                      const std::vector<BallDefinition>& Occupied,
                      std::vector<ColorRespotResult>& Output,std::string* Error=nullptr);
}
