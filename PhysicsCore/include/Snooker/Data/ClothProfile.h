#pragma once
#include <Snooker/Math/SnookerUnits.h>
#include <string>

namespace Snooker
{
struct ClothProfile
{
    std::string ParameterVersion;
    std::string Condition;
    Scalar Sliding=0, Rolling=0, Spin=0;
    Scalar SleepLinearSpeed=0, SleepAngularSpeed=0;
    Meters SleepPositionTolerance=0;
    Seconds SleepDuration=0;
    bool IsValid(std::string* Error=nullptr) const
    {
        const auto token=[](const std::string& v) { return !v.empty() && v.find_first_of(" \t\r\n")==std::string::npos; };
        if (!token(ParameterVersion) || !token(Condition)) {
            if(Error) *Error="cloth version and condition must be tokens";
            return false;
        }
        for(const Scalar v:{Sliding,Rolling,Spin,SleepLinearSpeed,SleepAngularSpeed,SleepPositionTolerance,SleepDuration})
            if(!IsFinite(v) || v<0) { if(Error) *Error="cloth parameters must be finite and nonnegative"; return false; }
        return true;
    }
};
}
