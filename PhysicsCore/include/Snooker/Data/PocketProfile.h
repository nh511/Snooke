#pragma once
#include <Snooker/Math/SnookerUnits.h>
#include <string>

namespace Snooker
{
struct PocketProfile
{
    std::string Id, ParameterVersion;
    Meters OpeningWidth=0, JawRadius=0;
    Radians JawAngle=0;
    Meters BottomDepth=0;
    Radians BottomSlope=0;
    Meters CaptureDepth=0;
    Scalar JawRestitution=0, EjectionSpeedThreshold=0;
    bool IsValid(std::string* Error=nullptr) const;
    std::string Serialize() const;
    static bool Deserialize(const std::string& Text,PocketProfile& Output,std::string* Error=nullptr);
};
}
