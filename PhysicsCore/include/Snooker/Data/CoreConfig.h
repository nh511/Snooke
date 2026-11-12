#pragma once
#include <Snooker/Data/BallDefinition.h>
#include <Snooker/Shot/CueProfile.h>
#include <string>
#include <vector>

namespace Snooker
{
struct TableParameters { Meters Length=0, Width=0; };
struct ClothParameters { Scalar Sliding=0, Rolling=0, Spin=0; };
struct CushionParameters { Scalar Restitution=0, TangentialFriction=0; Meters ContactHeight=0; };
struct PocketParameters { Meters Opening=0, JawRadius=0, Depth=0; };
struct SolverParameters
{
    Seconds FixedStep=0;
    std::uint32_t Iterations=0;
    Scalar SleepLinearSpeed=0, SleepAngularSpeed=0;
    Seconds SleepDuration=0;
};
struct CoreConfig
{
    std::string ParameterVersion;
    TableParameters Table;
    ClothParameters Cloth;
    CushionParameters Cushion;
    PocketParameters Pocket;
    FCueProfile Cue;
    SolverParameters Solver;
    std::vector<FBallDefinition> Balls;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text, CoreConfig& Output, std::string* Error=nullptr);
};
}
