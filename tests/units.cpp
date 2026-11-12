#include "Snooker/Math/SnookerUnits.h"
#include <iostream>

int main(int argc, char** argv)
{
    using namespace Snooker;
    if (argc == 3 && std::string(argv[1]) == "--expect-centimeters")
    {
        const double Expected = std::stod(argv[2]);
        if (!NearlyEqual(MetersToCentimeters(1.0), Expected))
        {
            std::cerr << "FAIL expected centimeters mismatch\n";
            return 1;
        }
    }
    else if (argc != 1) return 2;
    const FSnookerVector3 p{1.2, -0.4, 0.02625};
    const FSnookerVector3 w{0.3, 2.0, -1.0};
    const auto check = [](bool value) { if (!value) std::cerr << "unit conversion failure\n"; return value; };
    if (!check(NearlyEqual(CentimetersToMeters(MetersToCentimeters(-1.25)), -1.25)) ||
        !check(NearlyEqual(RadiansToDegrees(DegreesToRadians(93.0)), 93.0)) ||
        !check(UEPositionCentimetersToCore(CorePositionToUECentimeters(p)).NearlyEquals(p)) ||
        !check(UEDirectionToCore(CoreDirectionToUE(p)).NearlyEquals(p)) ||
        !check(UEAngularVelocityToCore(CoreAngularVelocityToUE(w)).NearlyEquals(w)) ||
        !check(CorePositionToUECentimeters({1, 2, 3}).NearlyEquals({100, -200, 300})) ||
        !check(CoreAngularVelocityToUE({1, 2, 3}).NearlyEquals({-1, 2, -3})) ||
        !check(CoreDirectionToUE(w.Cross(p)).NearlyEquals(
            CoreAngularVelocityToUE(w).Cross(CoreDirectionToUE(p))))) return 1;
    std::cout << "PASS units, handedness and angular cross-product consistency\n";
}
