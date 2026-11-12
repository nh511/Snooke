#pragma once
#include <Snooker/Math/SnookerUnits.h>
#include <string>

namespace Snooker
{
struct CushionProfile
{
    std::string Id;
    std::string ParameterVersion;
    Scalar Restitution=0;
    Scalar TangentialFriction=0;
    Meters ContactHeight=0;
    Meters PocketTransitionLength=0;
    Radians PocketTransitionAngle=0;

    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text, CushionProfile& Output, std::string* Error=nullptr);
};
}
