#include <Snooker/Data/ClothProfile.h>
#include <iostream>
#include <limits>
int main()
{
    using namespace Snooker;
    const ClothProfile fast{"cloth-fast-v1","dry",0.2,0.008,0.005,0.001,0.01,0.0001,0.25};
    auto slow=fast; slow.ParameterVersion="cloth-slow-v1"; slow.Condition="damp"; slow.Sliding=0.25; slow.Rolling=0.02;
    if(!fast.IsValid() || !slow.IsValid() || !(slow.Rolling>fast.Rolling)) return 1;
    auto invalid=fast; invalid.Sliding=-1; if(invalid.IsValid()) return 1;
    invalid=fast; invalid.Spin=std::numeric_limits<double>::quiet_NaN(); if(invalid.IsValid()) return 1;
    invalid=fast; invalid.ParameterVersion=""; if(invalid.IsValid()) return 1;
    invalid=fast; invalid.SleepDuration=-1; if(invalid.IsValid()) return 1;
    std::cout << "PASS independently switchable cloth conditions, version, friction and sleep parameter bounds\n";
}
