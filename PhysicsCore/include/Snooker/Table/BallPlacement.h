#pragma once
#include <Snooker/Data/TableDefinition.h>

namespace Snooker
{
struct OpeningLayout
{
    std::string ParameterVersion;
    Vec3d CuePosition, RedApex;
    Vec2d RackDirection{1,0};
    Meters RedClearance=0;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,OpeningLayout& Output,std::string* Error=nullptr);
};
// Canonical ID order, complete radius within cloth, correct center height, no overlaps.
bool GenerateCustomPlacement(const TableDefinition& Table,const std::vector<BallDefinition>& Input,
    std::vector<BallDefinition>& Output,std::string* Error=nullptr);
bool GenerateTrainingPlacement(const TableDefinition& Table,std::vector<BallDefinition>& Output,std::string* Error=nullptr);
// Exactly 15 reds + six colors + cue; radius/mass/material are explicit prototype data.
bool GenerateOpeningPlacement(const TableDefinition& Table,const BallDefinition& Prototype,const OpeningLayout& Layout,
    std::vector<BallDefinition>& Output,std::string* Error=nullptr);
// Spot-occupied is an explicit error; official alternative-spot rules belong to RUL-011.
bool ResetColorPlacement(const TableDefinition& Table,const BallDefinition& Color,
    const std::vector<BallDefinition>& Occupied,BallDefinition& Output,std::string* Error=nullptr);
}
